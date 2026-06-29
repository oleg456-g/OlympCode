"""
Изолированный судья для интерактивных задач.

Архитектура:
  - Решение запускается внутри isolate box
  - Интерактор запускается на хосте (доверенный код)
  - Общение через два named pipe (FIFO), bind-mounted внутрь box
  - Wall-таймер на стороне судьи
"""

import os
import shutil
import subprocess
import tempfile
import time
import signal
from pathlib import Path

from models import Verdict
from ioi_judge import IOIResult, TestDetail

def _isolate_cleanup(box_id):
    subprocess.run(["isolate", "--box-id", str(box_id), "--cleanup"],
                   capture_output=True, timeout=10)


# ── Компиляция (как в interactive_judge) ─────────────────────────────────────

def _compile_cpp(code: str, tmpdir: str) -> tuple[str, str | None]:
    src = os.path.join(tmpdir, "solution.cpp")
    exe = os.path.join(tmpdir, "solution")
    with open(src, "w", encoding="utf-8") as f:
        f.write(code)
    result = subprocess.run(
        ["g++", "-O2", "-std=c++20", "-o", exe, src],
        capture_output=True, text=True, timeout=30,
    )
    if result.returncode != 0:
        return "", result.stderr[:4096]
    return exe, None


# ── Один тест в isolate ──────────────────────────────────────────────────────

def _run_interactive_test_isolate(
    solution_cmd: list[str],
    interactor_path: str,
    in_file: Path,
    checker_path: str,
    time_limit: float,
    memory_limit: int,
    tmpdir: str,
    test_num: int,
    test_score: float,
    box_id: int,
) -> TestDetail:
    interactor_output = os.path.join(tmpdir, f"interact_out_{test_num}.txt")
    fifo_dir = os.path.join(tmpdir, f"fifos_{test_num}")
    os.makedirs(fifo_dir, exist_ok=True)

    fifo_sol_in  = os.path.join(fifo_dir, "sol_in")
    fifo_sol_out = os.path.join(fifo_dir, "sol_out")

    for f in (fifo_sol_in, fifo_sol_out):
        if os.path.exists(f):
            os.unlink(f)
        os.mkfifo(f, 0o666)
        os.chmod(f, 0o777)        # mkfifo режется umask, поэтому отдельный chmod
    os.chmod(fifo_dir, 0o777)     # ← директория тоже!

    subprocess.run(["isolate", "--box-id", str(box_id), "--cleanup"], capture_output=True)
    init = subprocess.run(
        ["isolate", "--box-id", str(box_id), "--init"],
        capture_output=True, text=True
    )
    if init.returncode != 0:
        return TestDetail(test_num, Verdict.RE, 0.0, 0.0)

    box_dir = init.stdout.strip() + "/box"
    meta_file = f"/tmp/isolate_meta_{box_id}.txt"
    if os.path.exists(meta_file):
        try:
            os.unlink(meta_file)
        except PermissionError:
            subprocess.run(["rm", "-f", meta_file], capture_output=True)

    is_python = solution_cmd[0].endswith("python3") or solution_cmd[0].endswith("python")
    if is_python:
        py_src = solution_cmd[1]
        shutil.copy2(py_src, os.path.join(box_dir, "solution.py"))
        inner_cmd = ["/usr/bin/python3", "-u", "solution.py"]
    else:
        exe = solution_cmd[0]
        dst = os.path.join(box_dir, "solution")
        shutil.copy2(exe, dst)
        os.chmod(dst, 0o755)
        inner_cmd = ["./solution"]

    isolate_cmd = [
        "isolate",
        "--box-id", str(box_id),
        f"--time={time_limit}",
        f"--wall-time={time_limit * 2}",
        f"--mem={memory_limit * 1024}",
        "--processes=10",
        "--stack=65536",
        f"--dir=/pipes={fifo_dir}:rw",
        f"--meta={meta_file}",
        "--stdin=/pipes/sol_in",
        "--stdout=/pipes/sol_out",
        "--stderr=/dev/null",
    ]
    # Динамические библиотеки нужны ВСЕГДА (и C++, и Python слинкованы динамически)
    isolate_cmd += ["--dir=/lib", "--dir=/lib64", "--dir=/usr/lib"]
    if is_python:
        isolate_cmd += ["--dir=/usr", "--dir=/etc"]
    isolate_cmd += ["--run", "--"] + inner_cmd

    start = time.perf_counter()

    proc_isolate = subprocess.Popen(
        isolate_cmd,
        stdin=subprocess.DEVNULL,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
    )

    # Оба конца открываем в O_RDWR.
    # Почему именно так: открытие read-конца FIFO в режиме O_RDONLY|O_NONBLOCK
    # создаёт гонку. Если родитель успевает открыть sol_out на чтение РАНЬШЕ,
    # чем решение в box откроет свой конец на запись, возникает окно, в котором
    # труба не имеет писателя, а полуоткрытые состояния sol_in/sol_out разъезжаются
    # по таймингу — обе стороны блокируются не вовремя относительно первого обмена.
    # Итог — недетерминированный дедлок (TLE на части прогонов, AC на других),
    # зависящий от того, кто кого опередил на микросекунды.
    #
    # O_RDWR на FIFO по семантике ядра НИКОГДА не блокируется на open (труба
    # сразу считается имеющей и читателя, и писателя) и не создаёт окна раннего
    # EOF. В отличие от блокирующего O_WRONLY, не зависнет навсегда, если решение
    # вообще не стартовало (например, бинарь не запустился) — родитель просто
    # пойдёт дальше, isolate отработает по wall-таймауту, и мы получим корректный
    # вердикт вместо вечного зависания.
    try:
        fd_read  = os.open(fifo_sol_out, os.O_RDWR)
        fd_write = os.open(fifo_sol_in, os.O_RDWR)
    except OSError as e:
        _isolate_cleanup(box_id)
        return TestDetail(test_num, Verdict.RE, 0.0, 0.0)

    proc_interactor = subprocess.Popen(
        [interactor_path, str(in_file), interactor_output],
        stdin=fd_read,
        stdout=fd_write,
        stderr=subprocess.DEVNULL,
        pass_fds=(fd_read, fd_write),
    )

    os.close(fd_read)
    os.close(fd_write)

    wall_limit = time_limit * 2
    deadline = start + wall_limit
    timed_out = False

    while time.perf_counter() < deadline:
        iso_done = proc_isolate.poll() is not None
        int_done = proc_interactor.poll() is not None

        if iso_done and int_done:
            break
        if iso_done and not int_done:
            extra = time.perf_counter() + 0.5
            while time.perf_counter() < extra:
                if proc_interactor.poll() is not None:
                    break
                time.sleep(0.05)
            if proc_interactor.poll() is None:
                proc_interactor.kill()
            break
        if int_done and not iso_done:
            extra = time.perf_counter() + 0.5
            while time.perf_counter() < extra:
                if proc_isolate.poll() is not None:
                    break
                time.sleep(0.05)
            if proc_isolate.poll() is None:
                _isolate_cleanup(box_id)   # ← вместо proc_isolate.kill()
            break
    else:
        timed_out = True
        proc_interactor.kill()
        # isolate убиваем правильно — через cleanup, он прибьёт процесс в box
        _isolate_cleanup(box_id)

    # Дожидаемся
    try:
        proc_isolate.wait(timeout=3)
    except subprocess.TimeoutExpired:
        _isolate_cleanup(box_id)
        try:
            proc_isolate.wait(timeout=2)
        except subprocess.TimeoutExpired:
            pass
    try:
        proc_interactor.wait(timeout=2)
    except subprocess.TimeoutExpired:
        proc_interactor.kill()

    # ГАРАНТИРОВАННЫЙ cleanup в любом случае
    _isolate_cleanup(box_id)

    elapsed = time.perf_counter() - start

    meta = {}
    if os.path.exists(meta_file):
        with open(meta_file) as f:
            for line in f:
                if ":" in line:
                    k, v = line.strip().split(":", 1)
                    meta[k] = v

    subprocess.run(["isolate", "--box-id", str(box_id), "--cleanup"], capture_output=True)
    for fp in (fifo_sol_in, fifo_sol_out):
        try:
            os.unlink(fp)
        except OSError:
            pass

    iso_status = meta.get("status", "")
    iso_time = float(meta.get("time", elapsed))

    if timed_out or iso_status == "TO":
        return TestDetail(test_num, Verdict.TLE, time_limit, 0.0)
    if iso_status == "ML":
        return TestDetail(test_num, Verdict.RE, iso_time, 0.0)
    if iso_status in ("RE", "SG"):
        return TestDetail(test_num, Verdict.RE, iso_time, 0.0)
    if iso_status == "XX":
        return TestDetail(test_num, Verdict.RE, iso_time, 0.0)

    int_rc = proc_interactor.returncode
    if int_rc is None:
        return TestDetail(test_num, Verdict.RE, iso_time, 0.0)
    if int_rc == 1 or int_rc == 2:
        return TestDetail(test_num, Verdict.WA, iso_time, 0.0)
    if int_rc in (3, 4):
        return TestDetail(test_num, Verdict.RE, iso_time, 0.0)
    if int_rc != 0 and int_rc != 7:
        return TestDetail(test_num, Verdict.RE, iso_time, 0.0)

    if not checker_path or not os.path.exists(checker_path):
        return TestDetail(test_num, Verdict.AC, iso_time, test_score)

    ans_file = None
    for suffix in (".a", ".ans", ".out"):
        candidate = str(in_file.with_suffix(suffix)) if in_file.suffix else str(in_file) + suffix
        if os.path.exists(candidate):
            ans_file = candidate
            break

    if ans_file is None:
        return TestDetail(test_num, Verdict.AC, iso_time, test_score)
    if not os.path.exists(interactor_output) or os.path.getsize(interactor_output) == 0:
        return TestDetail(test_num, Verdict.AC, iso_time, test_score)

    checker_res = subprocess.run(
        [checker_path, str(in_file), interactor_output, ans_file],
        capture_output=True, text=True,
    )
    if checker_res.returncode == 0:
        return TestDetail(test_num, Verdict.AC, iso_time, test_score)
    if checker_res.returncode in (1, 2):
        return TestDetail(test_num, Verdict.WA, iso_time, 0.0)
    return TestDetail(test_num, Verdict.RE, iso_time, 0.0)


# ── Главная функция (как в interactive_judge, но с isolate) ──────────────────

def judge_interactive_isolate(
    code: str,
    language: str,
    tests_path: str,
    time_limit: float,
    memory_limit: int,
    interactor_path: str,
    checker_path: str,
    test_scores: list[float],
    box_id: int = 0,
) -> IOIResult:
    max_score = sum(test_scores) if test_scores else 0.0
    with tempfile.TemporaryDirectory() as tmpdir:
        if language == "cpp":
            exe, compile_error = _compile_cpp(code, tmpdir)
            if compile_error:
                return IOIResult(verdict=Verdict.CE, score=0.0, max_score=max_score, error_output=compile_error)
            solution_cmd = [exe]
        elif language == "python":
            src = os.path.join(tmpdir, "solution.py")
            with open(src, "w", encoding="utf-8") as f:
                f.write(code)
            solution_cmd = ["/usr/bin/python3", src]
        else:
            return IOIResult(verdict=Verdict.CE, score=0.0, max_score=max_score, error_output=f"Не поддерживается: {language}")

        tests_dir = Path(tests_path)
        in_files = sorted(
            tests_dir.glob("*.in"),
            key=lambda p: int(p.stem) if p.stem.isdigit() else 0,
        )
        if not in_files:
            in_files = sorted(
                [f for f in tests_dir.iterdir() if f.is_file() and f.name.isdigit()],
                key=lambda p: int(p.name),
            )

        if not in_files:
            return IOIResult(verdict=Verdict.RE, score=0.0, max_score=max_score, error_output="Тесты не найдены")

        test_details = []
        total_score = 0.0
        max_time = 0.0
        first_error = None
        overall_verdict = Verdict.AC

        for in_file in in_files:
            test_num = int(in_file.stem) if in_file.stem.isdigit() else int(in_file.name)
            test_score = test_scores[test_num - 1] if 0 < test_num <= len(test_scores) else 0.0

            detail = _run_interactive_test_isolate(
                solution_cmd=solution_cmd,
                interactor_path=interactor_path,
                in_file=in_file,
                checker_path=checker_path,
                time_limit=time_limit,
                memory_limit=memory_limit,
                tmpdir=tmpdir,
                test_num=test_num,
                test_score=test_score,
                box_id=box_id,
            )

            test_details.append(detail)
            total_score += detail.score
            max_time = max(max_time, detail.execution_time or 0.0)

            if detail.verdict != Verdict.AC:
                if overall_verdict == Verdict.AC:
                    overall_verdict = detail.verdict
                    first_error = f"Тест {test_num}: {detail.verdict.value}"
                break

        final_verdict = Verdict.AC if total_score >= max_score and max_score > 0 else overall_verdict
        result = IOIResult(
            verdict=final_verdict,
            score=total_score,
            max_score=max_score,
            tests=test_details,
            error_output=first_error,
            execution_time=max_time if max_time > 0 else None,
        )
        return result