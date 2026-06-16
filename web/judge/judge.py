import os
import glob
import shutil
import time
import subprocess
import tempfile
from dataclasses import dataclass
from pathlib import Path
from models import Verdict


@dataclass
class JudgeResult:
    verdict:      Verdict
    failed_test:  int | None = None
    error_output: str | None = None
    execution_time: float | None = None


def judge(
    code: str,
    language: str,
    tests_path: str,
    time_limit: float,
    memory_limit: int,   # пока не используем без Docker, но храним для совместимости
    checker_path: str,
) -> JudgeResult:
    with tempfile.TemporaryDirectory() as tmpdir:
        if language == "cpp":
            return _judge_cpp(code, tests_path, time_limit, tmpdir, checker_path)
        elif language == "python":
            return _judge_python(code, tests_path, time_limit, tmpdir, checker_path)
        else:
            return JudgeResult(Verdict.CE, error_output=f"Язык не поддерживается: {language}")


# ── C++ ──────────────────────────────────────────────────────────────────────

def _judge_cpp(code: str, tests_path: str, time_limit: float, tmpdir: str, checker_path: str) -> JudgeResult:
    src = os.path.join(tmpdir, "solution.cpp")
    exe = os.path.join(tmpdir, "solution")
    
    # Для Windows g++ добавляет расширение .exe автоматически
    if os.name == 'nt':
        exe += ".exe"

    with open(src, "w", encoding="utf-8") as f:
        f.write(code)

    # Компиляция
    compile_result = subprocess.run(
        ["g++", "-O2", "-std=c++20", "-o", exe, src],
        capture_output=True, text=True, timeout=30
    )
    if compile_result.returncode != 0:
        return JudgeResult(
            Verdict.CE,
            error_output=compile_result.stderr[:4096]
        )

    return _run_tests(exe, None, tests_path, time_limit, tmpdir, checker_path)


# ── Python ───────────────────────────────────────────────────────────────────

def _judge_python(code: str, tests_path: str, time_limit: float, tmpdir: str, checker_path: str | None) -> JudgeResult:
    src = os.path.join(tmpdir, "solution.py")
    with open(src, "w", encoding="utf-8") as f:
        f.write(code)
    return _run_tests(None, src, tests_path, time_limit, tmpdir, checker_path)


# ── Общий прогон тестов ──────────────────────────────────────────────────────

def _run_tests(
    exe: str | None, 
    py_src: str | None, 
    tests_path: str, 
    time_limit: float, 
    tmpdir: str,
    checker_path: str | None
) -> JudgeResult:
    tests_dir = Path(tests_path)
    if not tests_dir.exists():
        return JudgeResult(Verdict.RE, error_output=f"Папка тестов не найдена: {tests_path}")

    # Ищем файлы .in
    in_files = sorted(
        tests_dir.glob("*.in"),
        key=lambda p: int(p.stem) if p.stem.isdigit() else 0
    )
    if not in_files:
        return JudgeResult(Verdict.RE, error_output="Тесты не найдены.")

    # Кроссплатформенный вызов python (в Windows python3 обычно нет)
    python_bin = "python" if os.name == "nt" else "python3"
    cmd = [exe] if exe else [python_bin, py_src]
    elapsed_time = 0.0
    for in_file in in_files:
        # У большинства платформ эталонный ответ — это *.ans или *.out. 
        # Проверим оба варианта, чтобы не падать
        out_file = in_file.with_suffix(".ans")
        if not out_file.exists():
            out_file = in_file.with_suffix(".out")
        if not out_file.exists():
            continue

        test_num = int(in_file.stem) if in_file.stem.isdigit() else 0

        # Запуск решения участника
        with open(in_file, "r", encoding="utf-8") as f_in:
            start_time = time.perf_counter()  # Стартуем высокоточный таймер
            
            try:
                result = subprocess.run(
                    cmd,
                    stdin=f_in,
                    capture_output=True,
                    text=True,
                    timeout=time_limit + 1.3,
                )
                # Считаем, сколько чистых секунд выполнялся процесс
                elapsed_time = time.perf_counter() - start_time
                
            except subprocess.TimeoutExpired:
                # Процесс жестко убит операционной системой по таймауту
                return JudgeResult(Verdict.TLE, failed_test=test_num, execution_time=time_limit)
            except Exception as e:
                return JudgeResult(Verdict.RE, failed_test=test_num, error_output=str(e), execution_time=elapsed_time)

        # Если рантайм-мир упал (деление на ноль, RE, segfault)
        if result.returncode != 0:
            return JudgeResult(
                Verdict.RE,
                failed_test=test_num,
                error_output=result.stderr[:2048],
                execution_time=elapsed_time,
            )

        # ПРОВЕРКА ОТВЕТА
        if checker_path and os.path.exists(checker_path):
            # Временный файл, куда сохраняем вывод участника для тестлиба
            team_out_path = os.path.join(tmpdir, "team.out")
            with open(team_out_path, "w", encoding="utf-8") as f:
                f.write(result.stdout)

            # Вызываем чекер testlib
            # Формат: checker.exe <input> <program-output> <target-answer>
            checker_res = subprocess.run(
                [checker_path, str(in_file), team_out_path, str(out_file)],
                capture_output=True,
                text=True
            )

            # Разбор exit codes у testlib.h:
            # 0 = OK, 1 = WA, 2 = PE (Presentation Error), 3 = FAIL (ошибка самого чекера)
            if checker_res.returncode == 0:
                continue
            elif checker_res.returncode == 1:
                return JudgeResult(Verdict.WA, failed_test=test_num, error_output=checker_res.stderr.strip(), execution_time=elapsed_time)
            elif checker_res.returncode == 2:
                # Можно обрабатывать отдельно как PE, либо как WA. В олимпиадах часто PE = WA
                return JudgeResult(Verdict.WA, failed_test=test_num, error_output=f"PE: {checker_res.stderr.strip()}", execution_time=elapsed_time)
            else:
                return JudgeResult(Verdict.RE, failed_test=test_num, error_output=f"Чекер сломался: {checker_res.stderr}", execution_time=elapsed_time)
        else:
            # Наш стандартный токенный дефолт, если чекер не задан
            expected = out_file.read_text(encoding="utf-8").strip()
            actual = result.stdout.strip()
            if not _compare(actual, expected):
                return JudgeResult(Verdict.WA, failed_test=test_num, execution_time=elapsed_time)

    return JudgeResult(Verdict.AC, failed_test=test_num, execution_time=elapsed_time)


def _compare(actual: str, expected: str) -> bool:
    """Токен-сравнение (не чувствительно к лишним пробелам и переносам)."""
    return actual.split() == expected.split()