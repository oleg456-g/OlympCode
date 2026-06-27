"""
Судья для интерактивных задач Polygon.

Запускает два процесса одновременно:
  - программу участника
  - интерактор (interact.exe / interactor)

Соединяет их stdout→stdin в обе стороны через два relay-потока.
После завершения запускает checker на output-файл интерактора.

Возвращает IOIResult (совместим и с ICPC и с IOI scoring):
  - ICPC: смотрим только verdict (AC/WA/TLE/RE/CE)
  - IOI:  смотрим score по каждому тесту
"""

import os
import subprocess
import tempfile
import threading
import time
from dataclasses import dataclass, field
from pathlib import Path

from models import Verdict

# Переиспользуем IOIResult и TestDetail из ioi_judge — структуры идентичны
from ioi_judge import IOIResult, TestDetail


# ── Компиляция ────────────────────────────────────────────────────────────────

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


# ── Relay-поток ───────────────────────────────────────────────────────────────

def _relay(src, dst, error_box: list):
    """
    Перекачивает байты из src в dst построчно.
    Работает в отдельном потоке.
    error_box — список для передачи ошибки наружу (mutable box pattern).
    """
    try:
        for line in src:
            try:
                dst.write(line)
                dst.flush()
            except (BrokenPipeError, OSError):
                # Получатель закрылся — нормальное завершение диалога
                break
    except Exception as e:
        error_box.append(str(e))
    finally:
        try:
            dst.close()
        except Exception:
            pass


# ── Один тест ─────────────────────────────────────────────────────────────────

def _run_interactive_test(
    solution_cmd: list[str],
    interactor_path: str,
    in_file: Path,
    checker_path: str,
    time_limit: float,
    memory_limit: int,
    tmpdir: str,
    test_num: int,
    test_score: float,
) -> TestDetail:
    """
    Запускает один тест интерактивной задачи.

    Интерактор вызывается как:
        interactor <input_file> <interactor_output_file>
    где <interactor_output_file> — файл куда интерактор пишет финальный
    результат через tout (он же идёт checker'у как ouf).

    Для интерактивных задач эталонного ответа (.a/.ans/.out) может не быть —
    в этом случае создаём пустой файл-заглушку для ans-аргумента checker'а.
    Большинство интерактивных checker'ов проверяют только ouf (результат
    от интерактора) и не используют ans, либо генерируют его сами.
    """
    interactor_output = os.path.join(tmpdir, f"interact_out_{test_num}.txt")

    # Выбираем бинарь интерактора — Linux или Windows
    if os.name == "nt":
        # На Windows ищем .exe рядом, если основной файл без расширения
        interactor_exe = interactor_path
        if not interactor_exe.endswith(".exe"):
            candidate = interactor_path + ".exe"
            if os.path.exists(candidate):
                interactor_exe = candidate
    else:
        interactor_exe = interactor_path

    if not os.path.exists(interactor_exe):
        return TestDetail(test_num, Verdict.RE, 0.0, 0.0)

    try:
        # Запускаем интерактор: читает из in_file, пишет результат в interactor_output
        proc_interactor = subprocess.Popen(
            [interactor_exe, str(in_file), interactor_output],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        def _set_limits():
            import resource
            mem_bytes = memory_limit * 1024 * 1024
            resource.setrlimit(resource.RLIMIT_AS, (mem_bytes, mem_bytes))
            resource.setrlimit(resource.RLIMIT_CPU, (int(time_limit) + 1, int(time_limit) + 2))

        # Запускаем решение участника
        proc_solution = subprocess.Popen(
            solution_cmd,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            preexec_fn=_set_limits if os.name != "nt" else None,
        )
    except OSError as e:
        return TestDetail(test_num, Verdict.RE, 0.0, 0.0)

    # Relay-потоки
    relay_errors: list[str] = []

    # solution.stdout → interactor.stdin
    t_sol_to_int = threading.Thread(
        target=_relay,
        args=(proc_solution.stdout, proc_interactor.stdin, relay_errors),
        daemon=True,
    )
    # interactor.stdout → solution.stdin
    t_int_to_sol = threading.Thread(
        target=_relay,
        args=(proc_interactor.stdout, proc_solution.stdin, relay_errors),
        daemon=True,
    )
    start = time.perf_counter()
    t_sol_to_int.start()
    t_int_to_sol.start()

    # Ждём завершения с таймаутом — проверяем каждые 50мс
    wall_limit = time_limit + 1.0
    timed_out = False

    deadline = start + wall_limit
    while time.perf_counter() < deadline:
        sol_done = proc_solution.poll() is not None
        int_done = proc_interactor.poll() is not None
        if sol_done and int_done:
            break
        time.sleep(0.05)
    else:
        timed_out = True
        _kill(proc_solution)
        _kill(proc_interactor)

    elapsed = time.perf_counter() - start

    # Принудительно закрываем stdout убитых процессов — это разблокирует
    # relay-потоки которые висят на `for line in src`. Без этого потоки
    # остаются живыми навсегда после kill (утечка потоков при каждом TLE).
    _close(proc_solution.stdout)
    _close(proc_interactor.stdout)
    _close(proc_solution.stdin)
    _close(proc_interactor.stdin)

    # Дожидаемся relay-потоков — теперь они гарантированно завершатся
    # потому что src закрыт и for line in src вернёт StopIteration
    t_sol_to_int.join(timeout=2.0)
    t_int_to_sol.join(timeout=2.0)

    if timed_out or elapsed > time_limit:
        return TestDetail(test_num, Verdict.TLE, time_limit, 0.0)

    # RE решения участника
    if proc_solution.returncode not in (0, None):
        stderr_sol = b""
        try:
            stderr_sol = proc_solution.stderr.read(2048)
        except Exception:
            pass
        error_msg = f"Solution RE (exit {proc_solution.returncode}): {stderr_sol.decode(errors='replace')[:500]}"
        return TestDetail(test_num, Verdict.RE, elapsed, 0.0)

    # Проверяем наличие output-файла интерактора
    if not os.path.exists(interactor_output):
        stderr_int = b""
        try:
            stderr_int = proc_interactor.stderr.read(1024)
        except Exception:
            pass
        error_msg = f"Interactor RE (exit {proc_interactor.returncode}), no output file. Stderr: {stderr_int.decode(errors='replace')[:500]}"
        return TestDetail(test_num, Verdict.RE, elapsed, 0.0)

    # Для интерактивных задач эталонного ответа (.a/.ans/.out) нет —
    # интерактор сам знает что правильно. Checker вызывается с
    # interactor_output как ouf (результат участника) и как ans тоже,
    # потому что большинство интерактивных checker'ов либо игнорируют ans,
    # либо используют его только для сравнения с ouf.
    # Поиск реального .a файла рядом с тестом (на случай если он есть).
    ans_file = interactor_output  # дефолт: ouf == ans
    for suffix in (".a", ".ans", ".out"):
        candidate = str(in_file) + suffix if not in_file.suffix else str(in_file.with_suffix(suffix))
        if os.path.exists(candidate):
            ans_file = candidate
            break

    # Запускаем checker
    score, verdict = _run_checker(
        checker_path, in_file, interactor_output, ans_file, test_score
    )
    return TestDetail(test_num, verdict, elapsed, score)


def _kill(proc: subprocess.Popen) -> None:
    try:
        proc.kill()
    except Exception:
        pass


def _close(stream) -> None:
    """Закрывает pipe-поток чтобы разблокировать relay-потоки висящие на чтении."""
    try:
        if stream and not stream.closed:
            stream.close()
    except Exception:
        pass


# ── Checker ───────────────────────────────────────────────────────────────────

TESTLIB_POINTS = 7  # exit code для quitp() в testlib.h

def _run_checker(
    checker_path: str,
    in_file: Path,
    interactor_output: str,
    answer_file: Path,
    test_max: float,
) -> tuple[float, Verdict]:
    """
    Запускает checker и возвращает (score, verdict).
    Поддерживает как обычный OK/WA так и quitp (частичные баллы).
    """
    if not checker_path or not os.path.exists(checker_path):
        # Нет чекера — сравниваем токенами
        got = Path(interactor_output).read_text(encoding="utf-8", errors="replace").strip()
        exp = answer_file.read_text(encoding="utf-8", errors="replace").strip()
        if got.split() == exp.split():
            return test_max, Verdict.AC
        return 0.0, Verdict.WA

    res = subprocess.run(
        [checker_path, str(in_file), interactor_output, str(answer_file)],
        capture_output=True, text=True,
    )
    ouf_content = Path(interactor_output).read_text(encoding="utf-8", errors="replace").strip()
    ans_content = Path(str(answer_file)).read_text(encoding="utf-8", errors="replace").strip()
    if res.returncode == TESTLIB_POINTS:
        # quitp — частичные баллы
        import re
        output = (res.stderr or res.stdout or "").strip()
        m = re.search(r"points[_\s]+([\d.]+)", output)
        if not m:
            # Пробуем взять первый токен
            try:
                score = float(output.split()[0].replace(",", "."))
            except (ValueError, IndexError):
                score = 0.0
        else:
            score = float(m.group(1))

        if test_max > 0 and score >= test_max - 1e-9:
            return test_max, Verdict.AC
        return score, Verdict.WA

    if res.returncode == 0:
        return test_max, Verdict.AC
    if res.returncode in (1, 2):
        return 0.0, Verdict.WA
    return 0.0, Verdict.RE


# ── Главная функция ───────────────────────────────────────────────────────────

def judge_interactive(
    code: str,
    language: str,
    tests_path: str,
    time_limit: float,
    memory_limit: int,
    interactor_path: str,
    checker_path: str,
    test_scores: list[float],   # балл за каждый тест (индекс = номер теста - 1)
) -> IOIResult:
    """
    Судья для интерактивных задач.

    Совместим с ICPC (смотри только verdict) и IOI (смотри score по тестам).
    """
    max_score = sum(test_scores) if test_scores else 0.0

    with tempfile.TemporaryDirectory() as tmpdir:
        # Компиляция
        if language == "cpp":
            exe, compile_error = _compile_cpp(code, tmpdir)
            if compile_error:
                return IOIResult(
                    verdict=Verdict.CE,
                    score=0.0,
                    max_score=max_score,
                    error_output=compile_error,
                )
            solution_cmd = [exe]
        elif language == "python":
            src = os.path.join(tmpdir, "solution.py")
            with open(src, "w", encoding="utf-8") as f:
                f.write(code)
            python_bin = "python" if os.name == "nt" else "python3"
            solution_cmd = [python_bin, src]
        else:
            return IOIResult(
                verdict=Verdict.CE,
                score=0.0,
                max_score=max_score,
                error_output=f"Язык не поддерживается: {language}",
            )

        # Находим тесты
        tests_dir = Path(tests_path)
        if not tests_dir.exists():
            return IOIResult(
                verdict=Verdict.RE,
                score=0.0,
                max_score=max_score,
                error_output=f"Папка тестов не найдена: {tests_path}",
            )

        # Поддерживаем оба формата: 01.in / 01 (без расширения)
        in_files = sorted(
            tests_dir.glob("*.in"),
            key=lambda p: int(p.stem) if p.stem.isdigit() else 0,
        )
        if not in_files:
            # Попробуем файлы без расширения (MOSH/Polygon-стиль)
            in_files = sorted(
                [f for f in tests_dir.iterdir() if f.is_file() and f.name.isdigit()],
                key=lambda p: int(p.name),
            )

        if not in_files:
            return IOIResult(
                verdict=Verdict.RE,
                score=0.0,
                max_score=max_score,
                error_output="Тесты не найдены",
            )

        test_details: list[TestDetail] = []
        total_score = 0.0
        max_time = 0.0
        first_error: str | None = None
        overall_verdict = Verdict.AC

        for in_file in in_files:
            test_num = int(in_file.stem) if in_file.stem.isdigit() else int(in_file.name)
            test_score = (
                test_scores[test_num - 1]
                if 0 < test_num <= len(test_scores)
                else 0.0
            )

            detail = _run_interactive_test(
                solution_cmd=solution_cmd,
                interactor_path=interactor_path,
                in_file=in_file,
                checker_path=checker_path,
                time_limit=time_limit,
                memory_limit=memory_limit,
                tmpdir=tmpdir,
                test_num=test_num,
                test_score=test_score,
            )

            test_details.append(detail)
            total_score += detail.score
            max_time = max(max_time, detail.execution_time or 0.0)

            # Для ICPC — останавливаемся на первой ошибке
            if detail.verdict not in (Verdict.AC,):
                if overall_verdict == Verdict.AC:
                    overall_verdict = detail.verdict
                    first_error = f"Тест {test_num}: {detail.verdict.value}"

        final_verdict = Verdict.AC if total_score >= max_score and max_score > 0 else overall_verdict

        return IOIResult(
            verdict=final_verdict,
            score=total_score,
            max_score=max_score,
            tests=test_details,
            error_output=first_error,
            execution_time=max_time if max_time > 0 else None,
        )