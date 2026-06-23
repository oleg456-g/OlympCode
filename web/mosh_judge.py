"""
MOSh-тестер: чекеры с quitp() возвращают частичные баллы (exit code 7).
Баллы за каждый тест суммируются.
"""

import os
import re
import time
import subprocess
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

from models import Verdict
from scoring_utils import zero_out_sample_tests

TESTLIB_POINTS = 7  # POINTS_EXIT_CODE в testlib.h (quitp)


@dataclass
class MoshTestDetail:
    test_number:    int
    verdict:        Verdict
    execution_time: float
    score:          float = 0.0
    max_score:      float = 0.0


@dataclass
class MoshResult:
    verdict:        Verdict
    score:          float
    max_score:      float
    tests:          list[MoshTestDetail] = field(default_factory=list)
    error_output:   str | None = None
    execution_time: float | None = None


def ensure_mosh_test_scores(
    checker_path: str,
    tests_path: str,
    test_scores: list[float],
    statement_html: str | None = None,
) -> list[float]:
    """Пересчитывает баллы и обнуляет примеры из условия."""
    if not test_scores or sum(test_scores) <= 0:
        if checker_path and tests_path:
            recomputed = compute_test_max_scores(checker_path, tests_path)
            if sum(recomputed) > 0:
                test_scores = recomputed

    if statement_html and test_scores:
        test_scores = zero_out_sample_tests(test_scores, statement_html, tests_path)

    return test_scores


def judge_mosh(
    code: str,
    language: str,
    tests_path: str,
    time_limit: float,
    memory_limit: int,
    checker_path: str,
    test_scores: list[float],
    is_output_only: bool = False,
) -> MoshResult:
    max_score = sum(test_scores) if test_scores else 0.0

    if is_output_only:
        return _judge_output_only(code, tests_path, checker_path, test_scores)

    with tempfile.TemporaryDirectory() as tmpdir:
        if language == "cpp":
            exe, compile_error = _compile_cpp(code, tmpdir)
            if compile_error:
                return MoshResult(Verdict.CE, 0, max_score, error_output=compile_error)
            cmd = [exe]
        elif language == "python":
            src = os.path.join(tmpdir, "solution.py")
            with open(src, "w", encoding="utf-8") as f:
                f.write(code)
            cmd = ["python" if os.name == "nt" else "python3", src]
        else:
            return MoshResult(Verdict.CE, 0, max_score,
                              error_output=f"Язык не поддерживается: {language}")

        return _run_mosh_tests(cmd, tmpdir, tests_path, time_limit, checker_path, test_scores)


def _judge_output_only(
    code: str,
    tests_path: str,
    checker_path: str,
    test_scores: list[float],
) -> MoshResult:
    """
    Output-only задача: участник сдаёт готовый текст ответа вместо кода.
    Программа не компилируется и не запускается — текст ответа подаётся
    в checker напрямую как вывод участника. В MOSH такие задачи всегда
    содержат ровно один тест.
    """
    max_score = sum(test_scores) if test_scores else 0.0

    tests_dir = Path(tests_path)
    if not tests_dir.exists():
        return MoshResult(Verdict.RE, 0, max_score,
                          error_output=f"Папка тестов не найдена: {tests_path}")

    in_files = _list_input_files(tests_dir)
    if not in_files:
        return MoshResult(Verdict.RE, 0, max_score, error_output="Тесты не найдены")

    in_file = in_files[0]
    out_file = _answer_file(in_file)
    if not out_file:
        return MoshResult(Verdict.RE, 0, max_score, error_output="Эталонный ответ не найден")

    test_num = int(in_file.stem) if in_file.stem.isdigit() else 1
    test_max = test_scores[test_num - 1] if 0 < test_num <= len(test_scores) else max_score

    with tempfile.TemporaryDirectory() as tmpdir:
        team_out = os.path.join(tmpdir, "team.out")
        with open(team_out, "w", encoding="utf-8") as f:
            f.write(code)

        earned, verdict = _run_checker(checker_path, in_file, team_out, out_file, test_max)

    detail = MoshTestDetail(test_num, verdict, 0.0, earned, test_max)
    return MoshResult(
        verdict   = verdict,
        score     = earned,
        max_score = max_score,
        tests     = [detail],
    )


def compute_test_max_scores(checker_path: str, all_tests: list[str], tests_path: str) -> list[float]:
    """
    Считает максимум баллов за каждый тест, подставляя ans как ответ участника.
    
    Важно: список возвращается по всем тестам на диске (включая samples),
    а не только по тестам из all_tests. Samples получают балл 0 — они не
    засчитываются участнику, но обязаны присутствовать в списке чтобы
    индекс test_num-1 совпадал с позицией в test_scores.
    """
    if not checker_path or not os.path.exists(checker_path):
        return []

    tests_dir = Path(tests_path)
    in_files = _list_input_files(tests_dir)
    
    # Нормализуем all_tests в set для быстрого поиска
    # Поддерживаем оба формата: 'tests/04' и просто '04'
    scoring_paths: set[str] = set()
    for input_path, _ in all_tests:
        if input_path:
            scoring_paths.add(input_path)
            # Добавляем и короткий вариант (только имя файла без папки)
            scoring_paths.add(Path(input_path).name)

    scores: list[float] = []
    
    with tempfile.TemporaryDirectory() as tmpdir:
        for in_file in in_files:
            p = Path(in_file)
            # Проверяем что тест входит в scoring набор (не sample)
            full_path  = f"{p.parent.name}/{p.stem}"   # 'tests/04'
            short_path = p.stem if p.suffix else p.name # '04'
            
            is_scoring = full_path in scoring_paths or short_path in scoring_paths
            
            if not is_scoring:
                # Sample тест — в баллах не участвует, ставим 0
                scores.append(0.0)
                continue

            out_file = _answer_file(in_file)
            if not out_file:
                scores.append(0.0)
                continue
            
            team_out = os.path.join(tmpdir, "team.out")
            shutil_copy(out_file, team_out)
            # Для quitp-чекеров (MOSH): earned содержит реальный балл независимо
            # от test_max. Для обычных OK/WA чекеров earned = test_max при rc=0,
            # поэтому передаём 0.0 — тогда rc=0 вернёт 0.0 (не используется для
            # compute, нас интересует только earned из quitp).
            # Если rc=0 (не quitp), считаем балл как 1.0 — тест пройден засчитан.
            earned, verdict = _run_checker(checker_path, in_file, team_out, out_file, 0.0)
            if verdict == Verdict.RE:
                scores.append(0.0)
            elif verdict == Verdict.AC and earned == 0.0:
                # rc=0 (обычный OK чекер без quitp) — тест пройден, но балл неизвестен
                # В этом контексте используем 1.0 как "тест засчитан"
                scores.append(1.0)
            else:
                scores.append(earned)

    return scores


def _compile_cpp(code: str, tmpdir: str) -> tuple[str, str | None]:
    src = os.path.join(tmpdir, "solution.cpp")
    exe = os.path.join(tmpdir, "solution")
    if os.name == "nt":
        exe += ".exe"
    with open(src, "w", encoding="utf-8") as f:
        f.write(code)
    result = subprocess.run(
        ["g++", "-O2", "-std=c++20", "-o", exe, src],
        capture_output=True, text=True, timeout=30,
    )
    if result.returncode != 0:
        return "", result.stderr[:4096]
    return exe, None


def _run_mosh_tests(
    cmd, tmpdir, tests_path, time_limit, checker_path, test_scores,
) -> MoshResult:
    tests_dir = Path(tests_path)
    if not tests_dir.exists():
        return MoshResult(Verdict.RE, 0, sum(test_scores),
                          error_output=f"Папка тестов не найдена: {tests_path}")

    in_files = _list_input_files(tests_dir)
    if not in_files:
        return MoshResult(Verdict.RE, 0, sum(test_scores), error_output="Тесты не найдены")

    max_score = sum(test_scores) if test_scores else 0.0
    details: list[MoshTestDetail] = []
    max_time = 0.0
    total_score = 0.0
    first_error: str | None = None

    for in_file in in_files:
        out_file = _answer_file(in_file)
        if not out_file:
            continue

        test_num = int(in_file.stem) if in_file.stem.isdigit() else 0
        test_max = test_scores[test_num - 1] if 0 < test_num <= len(test_scores) else 0.0
        if test_max <= 0:
            continue

        with open(in_file, "r", encoding="utf-8") as f_in:
            start = time.perf_counter()
            try:
                proc = subprocess.run(
                    cmd, stdin=f_in, capture_output=True,
                    text=True, timeout=time_limit + 1.3,
                )
                elapsed = time.perf_counter() - start
            except subprocess.TimeoutExpired:
                details.append(MoshTestDetail(test_num, Verdict.TLE, time_limit, 0.0, test_max))
                continue
            except Exception as e:
                details.append(MoshTestDetail(test_num, Verdict.RE, 0.0, 0.0, test_max))
                if not first_error:
                    first_error = str(e)
                continue

        max_time = max(max_time, elapsed)

        if proc.returncode != 0:
            details.append(MoshTestDetail(test_num, Verdict.RE, elapsed, 0.0, test_max))
            if not first_error:
                first_error = proc.stderr[:2048] or f"Exit code {proc.returncode}"
            continue

        team_out = os.path.join(tmpdir, f"team_{test_num}.out")
        with open(team_out, "w", encoding="utf-8") as f:
            f.write(proc.stdout)

        earned, verdict = _run_checker(checker_path, in_file, team_out, out_file, test_max)
        details.append(MoshTestDetail(test_num, verdict, elapsed, earned, test_max))
        total_score += earned

    final = Verdict.AC if total_score >= max_score and max_score > 0 else Verdict.WA
    return MoshResult(
        verdict        = final,
        score          = total_score,
        max_score      = max_score,
        tests          = details,
        error_output   = first_error,
        execution_time = max_time if max_time > 0 else None,
    )


def _run_checker(
    checker_path: str, in_file: Path, team_out: str, ans_file: Path,
    test_max: float = 0.0,
) -> tuple[float, Verdict]:
    if not checker_path or not os.path.exists(checker_path):
        return 0.0, Verdict.RE

    res = subprocess.run(
        [checker_path, str(in_file), team_out, str(ans_file)],
        capture_output=True, text=True,
    )
    output = (res.stderr or res.stdout or "").strip()

    if res.returncode == TESTLIB_POINTS:
        earned = _parse_checker_points(output)
        if test_max > 0 and earned >= test_max - 1e-9:
            return test_max, Verdict.AC
        if earned > 0:
            return earned, Verdict.WA
        return 0.0, Verdict.WA
    if res.returncode == 0:
        return test_max, Verdict.AC
    if res.returncode in (1, 2):
        return 0.0, Verdict.WA
    return 0.0, Verdict.RE


def _parse_checker_points(output: str) -> float:
    if not output:
        return 0.0
    if output.startswith("points_info="):
        part = output.split()[0].split("=", 1)[1]
        return float(part)
    m = re.search(r"points\s+([\d.,]+)", output)
    if m:
        return float(m.group(1).replace(",", "."))
    token = output.split()[0].replace(",", ".")
    try:
        return float(token)
    except ValueError:
        return 0.0


def _list_input_files(tests_dir: Path) -> list[Path]:
    in_files = sorted(tests_dir.glob("*.in"), key=lambda p: int(p.stem) if p.stem.isdigit() else 0)
    if in_files:
        return in_files
    # Polygon-формат без расширения
    result = []
    for f in sorted(tests_dir.iterdir(), key=lambda p: int(p.name) if p.name.isdigit() else 0):
        if f.is_file() and f.name.isdigit():
            result.append(f)
    return result


def _answer_file(in_file: Path) -> Path | None:
    for suffix in (".ans", ".out", ".a"):
        candidate = in_file.with_suffix(suffix)
        if candidate.exists():
            return candidate
    if in_file.suffix == ".in":
        bare = in_file.parent / in_file.stem
        if bare.exists() and bare.suffix == "":
            pass
    # Polygon: 01.in -> 01.out, or file "01" -> "01.a"
    stem = in_file.stem if in_file.suffix else in_file.name
    for ext in (".out", ".a", ".ans"):
        p = in_file.parent / f"{stem}{ext}"
        if p.exists():
            return p
    a_file = in_file.parent / f"{stem}.a"
    if a_file.exists():
        return a_file
    return None


def shutil_copy(src: Path, dst: str) -> None:
    with open(src, "r", encoding="utf-8") as f:
        content = f.read()
    with open(dst, "w", encoding="utf-8") as f:
        f.write(content)