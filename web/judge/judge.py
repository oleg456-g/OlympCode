import os
import glob
import shutil
import time
import subprocess
import tempfile
from dataclasses import dataclass
from pathlib import Path
from models import Verdict

USE_ISOLATE = True

@dataclass
class JudgeResult:
    verdict:        Verdict
    failed_test:    int | None   = None
    error_output:   str | None   = None
    execution_time: float | None = None


def judge(
    code: str,
    language: str,
    tests_path: str,
    time_limit: float,
    memory_limit: int,
    checker_path: str,
    on_test_start=None,
    box_id=0,
) -> JudgeResult:
    with tempfile.TemporaryDirectory() as tmpdir:
        if language == "cpp":
            return _judge_cpp(code, tests_path, time_limit, memory_limit, tmpdir, checker_path, on_test_start, box_id)
        elif language == "python":
            return _judge_python(code, tests_path, time_limit, memory_limit, tmpdir, checker_path, on_test_start, box_id)
        else:
            return JudgeResult(Verdict.CE, error_output=f"Язык не поддерживается: {language}")


# ── C++ ──────────────────────────────────────────────────────────────────────

def _judge_cpp(code, tests_path, time_limit, memory_limit, tmpdir, checker_path, on_test_start=None, box_id=0):
    src = os.path.join(tmpdir, "solution.cpp")
    exe = os.path.join(tmpdir, "solution")
    if os.name == 'nt':
        exe += ".exe"
    with open(src, "w", encoding="utf-8") as f:
        f.write(code)
    compile_result = subprocess.run(
        ["g++", "-O2", "-std=c++20", "-o", exe, src],
        capture_output=True, text=True, timeout=30
    )
    if compile_result.returncode != 0:
        return JudgeResult(Verdict.CE, error_output=compile_result.stderr[:4096])
    return _run_tests(exe, None, tests_path, time_limit, memory_limit, tmpdir, checker_path, on_test_start, box_id)


def _judge_python(code, tests_path, time_limit, memory_limit, tmpdir, checker_path, on_test_start=None, box_id=0):
    src = os.path.join(tmpdir, "solution.py")
    with open(src, "w", encoding="utf-8") as f:
        f.write(code)
    return _run_tests(None, src, tests_path, time_limit, memory_limit, tmpdir, checker_path, on_test_start, box_id)


# ── Запуск через isolate ──────────────────────────────────────────────────────

def _run_with_isolate(cmd_args, in_file, tmpdir, time_limit, memory_limit, box_id):
    meta_file = f"/tmp/isolate_meta_{box_id}.txt"

    subprocess.run(["isolate", "--box-id", str(box_id), "--cleanup"], capture_output=True)
    init = subprocess.run(
        ["isolate", "--box-id", str(box_id), "--init"],
        capture_output=True, text=True
    )
    box_dir = init.stdout.strip() + "/box"


    for f in cmd_args:
        if os.path.isfile(f):
            dst = os.path.join(box_dir, os.path.basename(f))
            shutil.copy2(f, dst)
            os.chmod(dst, 0o755)

    shutil.copy2(in_file, os.path.join(box_dir, "input.txt"))

    inner_cmd = [os.path.basename(f) if os.path.isfile(f) else f for f in cmd_args]

    isolate_cmd = [
        "isolate",
        "--box-id", str(box_id),
        f"--time={time_limit}",
        f"--wall-time={time_limit * 2 + 1}",
        f"--mem={memory_limit * 1024}",
        "--processes=1",
        "--stdin=input.txt",
        "--stdout=team.out",
        "--stderr=team.err",
        f"--meta={meta_file}",
        "--run", "--"
    ] + inner_cmd

    start = time.perf_counter()
    res = subprocess.run(isolate_cmd, capture_output=True, text=True)
    elapsed = time.perf_counter() - start

    out_path = os.path.join(box_dir, "team.out")
    err_path = os.path.join(box_dir, "team.err")
    stdout = open(out_path).read() if os.path.exists(out_path) else ""
    stderr = open(err_path).read() if os.path.exists(err_path) else ""

    meta = {}
    if os.path.exists(meta_file):
        for line in open(meta_file):
            if ":" in line:
                k, v = line.strip().split(":", 1)
                meta[k] = v


    status = meta.get("status", "")
    exitcode_meta = meta.get("exitcode", "")
    wall_time = float(meta.get("time-wall", elapsed))

    if status == "TO":
        returncode = -1   # TLE
    elif status in ("RE", "SG"):
        returncode = 1    # RE
    elif status == "XX":
        returncode = 2    # Internal error (настоящий)
    elif exitcode_meta == "0":
        returncode = 0    # ← программа отработала успешно, plevoy на returncode isolate
    elif exitcode_meta and exitcode_meta != "0":
        returncode = 1    # программа упала с ненулевым кодом
    else:
        # meta вообще не заполнен — вот тогда это настоящий XX
        returncode = 0 if res.returncode == 0 else 2

    subprocess.run(["isolate", "--box-id", str(box_id), "--cleanup"], capture_output=True)
    class FakeResult:
        pass
    r = FakeResult()
    r.returncode = returncode
    r.stdout = stdout
    r.stderr = stderr
    r.meta = meta
    r.isolate_stderr = res.stderr
    return r, wall_time


# ── Основной цикл тестов ─────────────────────────────────────────────────────

def _run_tests(exe, py_src, tests_path, time_limit, memory_limit, tmpdir, checker_path, on_test_start=None, box_id=0):
    tests_dir = Path(tests_path)
    if not tests_dir.exists():
        return JudgeResult(Verdict.RE, error_output=f"Папка тестов не найдена: {tests_path}")

    in_files = sorted(
        tests_dir.glob("*.in"),
        key=lambda p: int(p.stem) if p.stem.isdigit() else 0
    )
    if not in_files:
        return JudgeResult(Verdict.RE, error_output="Тесты не найдены.")

    python_bin = "python" if os.name == "nt" else "python3"
    cmd = [exe] if exe else [python_bin, py_src]
    elapsed_time = 0.0
    test_num = 0

    for in_file in in_files:
        out_file = in_file.with_suffix(".ans")
        if not out_file.exists():
            out_file = in_file.with_suffix(".out")
        if not out_file.exists():
            continue

        test_num = int(in_file.stem) if in_file.stem.isdigit() else 0

        if on_test_start:
            try:
                on_test_start(test_num)
            except Exception:
                pass

        # ── Запуск (isolate или обычный) ─────────────────────────────────────
        if USE_ISOLATE and os.name != "nt":
            result, elapsed_time = _run_with_isolate(cmd, str(in_file), tmpdir, time_limit, memory_limit, box_id)
            if result.returncode == -1:
                return JudgeResult(Verdict.TLE, failed_test=test_num, execution_time=time_limit)
            if result.returncode != 0:
                return JudgeResult(Verdict.RE, failed_test=test_num, execution_time=elapsed_time)

        else:
            with open(in_file, "r", encoding="utf-8") as f_in:
                start_time = time.perf_counter()
                try:
                    result = subprocess.run(
                        cmd,
                        stdin=f_in,
                        capture_output=True,
                        text=True,
                        timeout=time_limit + 1.3,
                    )
                    elapsed_time = time.perf_counter() - start_time
                except subprocess.TimeoutExpired:
                    return JudgeResult(Verdict.TLE, failed_test=test_num, execution_time=time_limit)
                except Exception as e:
                    return JudgeResult(Verdict.RE, failed_test=test_num, execution_time=elapsed_time)

            if result.returncode != 0:
                return JudgeResult(Verdict.RE, failed_test=test_num, execution_time=elapsed_time)

        # ── Проверка ответа ───────────────────────────────────────────────────
        if checker_path and os.path.exists(checker_path):
            team_out_path = os.path.join(tmpdir, "team.out")
            with open(team_out_path, "w", encoding="utf-8") as f:
                f.write(result.stdout)

            checker_res = subprocess.run(
                [checker_path, str(in_file), team_out_path, str(out_file)],
                capture_output=True, text=True
            )
            if checker_res.returncode == 0:
                continue
            elif checker_res.returncode in (1, 2):
                return JudgeResult(Verdict.WA, failed_test=test_num, execution_time=elapsed_time)
            else:
                return JudgeResult(Verdict.RE, failed_test=test_num, execution_time=elapsed_time)
        else:
            expected = out_file.read_text(encoding="utf-8").strip()
            actual = result.stdout.strip()
            if not _compare(actual, expected):
                return JudgeResult(Verdict.WA, failed_test=test_num, execution_time=elapsed_time)

    return JudgeResult(Verdict.AC, failed_test=test_num, execution_time=elapsed_time)


def _compare(actual: str, expected: str) -> bool:
    return actual.split() == expected.split()