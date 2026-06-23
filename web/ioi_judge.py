"""
IOI-тестер: прогоняет все тесты и считает баллы.

Два режима:
  - ioi_sum:    балл за каждый пройденный тест суммируется независимо
  - ioi_groups: подзадача засчитывается только если ВСЕ тесты в ней прошли
"""

import os
import time
import subprocess
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

from models import Verdict, ScoringType


@dataclass
class TestDetail:
    test_number:    int
    verdict:        Verdict
    execution_time: float
    score:          float = 0.0


@dataclass
class IOIResult:
    verdict:        Verdict          # итоговый: AC если макс баллов, иначе WA
    score:          float            # набранные баллы
    max_score:      float            # максимально возможные
    tests:          list[TestDetail] = field(default_factory=list)
    error_output:   str | None       = None
    execution_time: float | None     = None  # максимальное время среди всех тестов


def judge_ioi(
    code: str,
    language: str,
    tests_path: str,
    time_limit: float,
    memory_limit: int,
    checker_path: str,
    scoring_type: ScoringType,
    test_scores: list[float],
    subtasks: list | None,
    on_test_start=None,   # callback(test_num: int) — вызывается перед каждым тестом
) -> IOIResult:
    with tempfile.TemporaryDirectory() as tmpdir:
        # Компилируем
        if language == "cpp":
            exe, compile_error = _compile_cpp(code, tmpdir)
            if compile_error:
                return IOIResult(
                    verdict=Verdict.CE,
                    score=0, max_score=sum(test_scores),
                    error_output=compile_error
                )
            cmd = [exe]
        elif language == "python":
            src = os.path.join(tmpdir, "solution.py")
            with open(src, "w", encoding="utf-8") as f:
                f.write(code)
            cmd = ["python" if os.name == "nt" else "python3", src]
        else:
            return IOIResult(Verdict.CE, 0, sum(test_scores),
                             error_output=f"Язык не поддерживается: {language}")

        return _run_ioi_tests(
            cmd, tmpdir, tests_path, time_limit,
            checker_path, scoring_type, test_scores, subtasks,
            on_test_start=on_test_start,
        )


def _compile_cpp(code: str, tmpdir: str) -> tuple[str, str | None]:
    src = os.path.join(tmpdir, "solution.cpp")
    exe = os.path.join(tmpdir, "solution")
    if os.name == "nt":
        exe += ".exe"
    with open(src, "w", encoding="utf-8") as f:
        f.write(code)
    result = subprocess.run(
        ["g++", "-O2", "-std=c++20", "-o", exe, src],
        capture_output=True, text=True, timeout=30
    )
    if result.returncode != 0:
        return "", result.stderr[:4096]
    return exe, None


def _run_ioi_tests(
    cmd, tmpdir, tests_path, time_limit,
    checker_path, scoring_type, test_scores, subtasks,
    on_test_start=None,
) -> IOIResult:
    tests_dir = Path(tests_path)
    if not tests_dir.exists():
        return IOIResult(Verdict.RE, 0, sum(test_scores),
                         error_output=f"Папка тестов не найдена: {tests_path}")

    in_files = sorted(
        tests_dir.glob("*.in"),
        key=lambda p: int(p.stem) if p.stem.isdigit() else 0
    )
    if not in_files:
        # Поддержка .dat формата (например Яндекс.Контест / ejudge)
        in_files = sorted(
            tests_dir.glob("*.dat"),
            key=lambda p: int(p.stem) if p.stem.isdigit() else 0
        )
    if not in_files:
        # Числовые файлы без расширения (Polygon-стиль)
        in_files = sorted(
            [f for f in tests_dir.iterdir() if f.is_file() and f.name.isdigit()],
            key=lambda p: int(p.name)
        )
    if not in_files:
        return IOIResult(Verdict.RE, 0, sum(test_scores), error_output="Тесты не найдены")

    max_score   = sum(test_scores)
    # Для IOI_GROUPS реальный максимум включает баллы complete-group подзадач
    # которые хранятся в Subtask.max_score, а не в test_scores (там 0)
    if scoring_type == ScoringType.IOI_GROUPS and subtasks:
        max_score = sum(st.max_score for st in subtasks)
    test_details: list[TestDetail] = []
    max_time = 0.0

    # Для IOI_GROUPS нужно запускать ВСЕ тесты входящие в Subtask,
    # даже если test_score == 0 (complete-group группы хранят балл в
    # Subtask.max_score, а не в test_scores). Иначе complete-group тесты
    # будут пропущены и при проверке зависимостей окажется что группа
    # "не прошла" → зависящие группы получат 0 баллов.
    if scoring_type == ScoringType.IOI_GROUPS and subtasks:
        subtask_test_nums: set[int] = set()
        for st in subtasks:
            for n in range(st.test_from, st.test_to + 1):
                subtask_test_nums.add(n)
    else:
        subtask_test_nums = set()

    # Для IOI_GROUPS: определяем какие группы можно пропустить из-за
    # неудовлетворённых зависимостей ДО начала тестирования.
    # Это избавляет от бессмысленного запуска тестов, которые всё равно
    # не дадут баллов из-за того что зависимые группы ещё не пройдены.
    import json as _json
    skipped_subtask_nums: set[int] = set()
    if scoring_type == ScoringType.IOI_GROUPS and subtasks:
        # Группы которые имеют зависимости, нужно проверять по-прежнему
        # их тесты — нам нужно знать пройдены ли они сами. Пропускаем
        # только те группы, у которых ВСЕ зависимые группы уже заведомо
        # не могут быть пройдены (т.к. мы их тоже пропускаем).
        # Итерируем по топологическому порядку (предполагаем что Subtask
        # хранится в порядке номеров, и зависимости всегда на меньшие номера).
        for st in sorted(subtasks, key=lambda s: s.number):
            deps = _json.loads(st.depends_on_json) if st.depends_on_json else []
            if any(dep in skipped_subtask_nums for dep in deps):
                skipped_subtask_nums.add(st.number)

    for in_file in in_files:
        out_file = in_file.with_suffix(".ans")
        if not out_file.exists():
            out_file = in_file.with_suffix(".out")
        if not out_file.exists():
            continue

        test_num = int(in_file.stem) if in_file.stem.isdigit() else 0
        test_score = test_scores[test_num - 1] if 0 < test_num <= len(test_scores) else 0.0

        if test_score <= 0 and test_num not in subtask_test_nums:
            continue

        # Пропускаем тест если его группа зависит от непройденных групп.
        # Тест получит WA автоматически (он не в test_details → score=0).
        if scoring_type == ScoringType.IOI_GROUPS and subtasks and skipped_subtask_nums:
            test_subtask = next(
                (st for st in subtasks if st.test_from <= test_num <= st.test_to), None
            )
            if test_subtask and test_subtask.number in skipped_subtask_nums:
                test_details.append(TestDetail(test_num, Verdict.WA, 0.0, 0.0))
                continue

        # Уведомляем о начале теста
        if on_test_start:
            try:
                on_test_start(test_num)
            except Exception:
                pass

        with open(in_file, "r", encoding="utf-8") as f_in:
            start = time.perf_counter()
            try:
                proc = subprocess.run(
                    cmd, stdin=f_in, capture_output=True,
                    text=True, timeout=time_limit
                )
                elapsed = time.perf_counter() - start
            except subprocess.TimeoutExpired:
                test_details.append(TestDetail(test_num, Verdict.TLE, time_limit, 0.0))
                continue
            except Exception as e:
                test_details.append(TestDetail(test_num, Verdict.RE, 0.0, 0.0))
                continue

        max_time = max(max_time, elapsed)

        if proc.returncode != 0:
            test_details.append(TestDetail(test_num, Verdict.RE, elapsed, 0.0))
            continue

        # Проверяем ответ
        verdict = _check_answer(
            in_file, proc.stdout, out_file, checker_path, tmpdir
        )
        earned = test_score if verdict == Verdict.AC else 0.0
        test_details.append(TestDetail(test_num, verdict, elapsed, earned))

    # Считаем итоговый балл
    if scoring_type == ScoringType.IOI_SUM:
        total_score = sum(t.score for t in test_details)

    elif scoring_type == ScoringType.IOI_GROUPS and subtasks:
        import json

        # Первый проход: для каждой подзадачи определяем, пройдена ли она
        # сама (все её тесты — AC), независимо от зависимостей. Нужно
        # знать это для ВСЕХ подзадач заранее, потому что зависимости
        # могут ссылаться на любую другую подзадачу, а порядок в списке
        # subtasks не гарантированно совпадает с порядком зависимостей.
        passed_by_number: dict[int, bool] = {}
        tests_by_number: dict[int, list] = {}
        for st in subtasks:
            group_tests = [t for t in test_details if st.test_from <= t.test_number <= st.test_to]
            tests_by_number[st.number] = group_tests
            passed_by_number[st.number] = bool(group_tests) and all(
                t.verdict == Verdict.AC for t in group_tests
            )

        # Второй проход: начисляем баллы только если подзадача сама
        # пройдена И все подзадачи, от которых она зависит, тоже пройдены.
        # Без этой проверки участник мог бы получить бонус за сложную
        # подзадачу, не решив более простые, от которых она зависит —
        # что не соответствует тому, как Polygon/IOI размечает группы.
        total_score = 0.0
        for st in subtasks:
            group_tests = tests_by_number[st.number]
            if not passed_by_number[st.number]:
                continue

            depends_on = json.loads(st.depends_on_json) if st.depends_on_json else []
            deps_satisfied = all(passed_by_number.get(dep_num, False) for dep_num in depends_on)
            if not deps_satisfied:
                continue

            total_score += st.max_score
            # Проставляем балл подзадачи равномерно (для отображения)
            per_test = st.max_score / len(group_tests)
            for t in group_tests:
                t.score = per_test
    else:
        total_score = sum(t.score for t in test_details)

    # Итоговый вердикт
    final_verdict = Verdict.AC if total_score >= max_score else Verdict.WA

    return IOIResult(
        verdict        = final_verdict,
        score          = total_score,
        max_score      = max_score,
        tests          = test_details,
        execution_time = max_time if max_time > 0 else None,
    )


def _check_answer(in_file, stdout, out_file, checker_path, tmpdir) -> Verdict:
    if checker_path and os.path.exists(checker_path):
        team_out = os.path.join(tmpdir, "team.out")
        with open(team_out, "w", encoding="utf-8") as f:
            f.write(stdout)
        res = subprocess.run(
            [checker_path, str(in_file), team_out, str(out_file)],
            capture_output=True, text=True
        )
        if res.returncode == 0:
            return Verdict.AC
        elif res.returncode in (1, 2):
            return Verdict.WA
        else:
            return Verdict.RE
    else:
        expected = out_file.read_text(encoding="utf-8").strip()
        actual   = stdout.strip()
        return Verdict.AC if actual.split() == expected.split() else Verdict.WA