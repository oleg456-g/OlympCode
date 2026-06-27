"""
Персистентная очередь тестирования через БД.

Принцип:
  - Посылка создаётся с verdict=PENDING
  - Воркер-поллер берёт PENDING по одной (или N параллельно), ставит RUNNING, тестирует
  - При старте сервера все RUNNING → PENDING (восстановление после сбоя)
  - Если сервер упал во время тестирования — при следующем старте посылка
    автоматически уйдёт в повторную проверку

Параллелизм контролируется MAX_WORKERS — семафором на уровне потоков.
"""

import json
import threading
import time
import traceback
import queue
from datetime import datetime

MAX_WORKERS = 2  # сколько посылок проверять одновременно

_box_pool = queue.Queue()
for i in range(MAX_WORKERS):
    _box_pool.put(i)

# In-memory хранилище текущего теста для RUNNING посылок
# { submission_id: test_number }  — сбрасывается при завершении
_current_tests: dict[int, int] = {}
_current_tests_lock = threading.Lock()


def set_current_test(submission_id: int, test_num: int) -> None:
    with _current_tests_lock:
        _current_tests[submission_id] = test_num


def get_current_test(submission_id: int) -> int | None:
    with _current_tests_lock:
        return _current_tests.get(submission_id)


def clear_current_test(submission_id: int) -> None:
    with _current_tests_lock:
        _current_tests.pop(submission_id, None)


# ── Семафор ограничения параллелизма ────────────────────────────────────────

_semaphore = threading.Semaphore(MAX_WORKERS)


# ── Основной поллер ──────────────────────────────────────────────────────────

def start_judge_worker():
    """Запускает фоновый поток-поллер. Вызывать один раз при старте сервера."""
    t = threading.Thread(target=_poll_loop, daemon=True, name="judge-worker")
    t.start()


def _poll_loop():
    """Бесконечный цикл: берём PENDING посылки из БД и запускаем тестирование."""
    from database import SessionLocal
    from models import Submission, Verdict

    while True:
        try:
            db = SessionLocal()
            try:
                # Берём старейшую PENDING посылку
                sub = (
                    db.query(Submission)
                    .filter(Submission.verdict == Verdict.PENDING)
                    .order_by(Submission.submitted_at.asc())
                    .first()
                )
                if sub is None:
                    time.sleep(1.0)
                    continue

                # Атомарно помечаем как RUNNING чтобы другой воркер не взял
                sub.verdict = Verdict.RUNNING
                db.commit()
                sub_id = sub.id
            finally:
                db.close()

            # Запускаем тестирование в отдельном потоке с ограничением параллелизма
            t = threading.Thread(
                target=_run_with_semaphore,
                args=(sub_id,),
                daemon=True,
                name=f"judge-{sub_id}",
            )
            t.start()

            # Небольшая пауза чтобы не брать следующую посылку мгновенно
            # пока семафор не освободится
            time.sleep(0.2)

        except Exception:
            traceback.print_exc()
            time.sleep(2.0)


def _run_with_semaphore(submission_id: int):
    """Ожидает свободного слота и запускает _run_judge."""
    _semaphore.acquire()
    box_id = _box_pool.get()
    try:
        _run_judge(submission_id, box_id=box_id)
    finally:
        _box_pool.put(box_id)
        _semaphore.release()


# ── Логика тестирования (перенесена из submissions.py) ───────────────────────

def _run_judge(submission_id: int, box_id: int) -> None:
    from database import SessionLocal
    from models import Submission, Task, Verdict, ScoringType, TestResult
    from judge.judge import judge
    from ioi_judge import judge_ioi
    from mosh_judge import judge_mosh
    from interactive_judge import judge_interactive

    db = SessionLocal()
    try:
        sub  = db.query(Submission).filter_by(id=submission_id).first()
        if not sub:
            return
        task = db.query(Task).filter_by(id=sub.task_id).first()
        if not task:
            sub.verdict = Verdict.RE
            sub.error_output = "Задача не найдена"
            db.commit()
            return

        # Убеждаемся что статус RUNNING (на случай повторного запуска)
        sub.verdict = Verdict.RUNNING
        db.commit()

        test_scores = json.loads(task.test_scores_json) if task.test_scores_json else []

        def on_test_start(test_num: int):
            """Вызывается судьёй перед каждым тестом."""
            set_current_test(submission_id, test_num)

        if task.is_interactive and task.interactor_path:
            result = judge_interactive(
                code            = sub.code,
                language        = sub.language,
                tests_path      = task.tests_path,
                time_limit      = task.time_limit,
                memory_limit    = task.memory_limit,
                interactor_path = task.interactor_path,
                checker_path    = task.checker_path,
                test_scores     = test_scores,
            )
            sub.verdict        = result.verdict
            sub.score          = result.score
            sub.execution_time = result.execution_time
            sub.error_output   = result.error_output
            for t in result.tests:
                db.add(TestResult(
                    submission_id  = sub.id,
                    test_number    = t.test_number,
                    verdict        = t.verdict,
                    execution_time = t.execution_time,
                    score          = t.score,
                ))

        elif task.scoring_type == ScoringType.MOSH:
            result = judge_mosh(
                code           = sub.code,
                language       = sub.language,
                tests_path     = task.tests_path,
                time_limit     = task.time_limit,
                memory_limit   = task.memory_limit,
                checker_path   = task.checker_path,
                test_scores    = test_scores,
                is_output_only = task.is_output_only,
            )
            sub.verdict        = result.verdict
            sub.score          = result.score
            sub.execution_time = result.execution_time
            sub.error_output   = result.error_output
            for t in result.tests:
                db.add(TestResult(
                    submission_id  = sub.id,
                    test_number    = t.test_number,
                    verdict        = t.verdict,
                    execution_time = t.execution_time,
                    score          = t.score,
                ))

        elif task.scoring_type in (ScoringType.IOI_SUM, ScoringType.IOI_GROUPS):
            subtasks = task.subtasks if task.scoring_type == ScoringType.IOI_GROUPS else None
            result = judge_ioi(
                code          = sub.code,
                language      = sub.language,
                tests_path    = task.tests_path,
                time_limit    = task.time_limit,
                memory_limit  = task.memory_limit,
                checker_path  = task.checker_path,
                scoring_type  = task.scoring_type,
                test_scores   = test_scores,
                subtasks      = subtasks,
                on_test_start = on_test_start,
            )
            sub.verdict        = result.verdict
            sub.score          = result.score
            sub.execution_time = result.execution_time
            sub.error_output   = result.error_output
            for t in result.tests:
                db.add(TestResult(
                    submission_id  = sub.id,
                    test_number    = t.test_number,
                    verdict        = t.verdict,
                    execution_time = t.execution_time,
                    score          = t.score,
                ))

        else:
            result = judge(
                code          = sub.code,
                language      = sub.language,
                tests_path    = task.tests_path,
                time_limit    = task.time_limit,
                memory_limit  = task.memory_limit,
                checker_path  = task.checker_path,
                on_test_start = on_test_start,
                box_id        = box_id,
            )
            sub.verdict        = result.verdict
            sub.failed_test    = result.failed_test
            sub.error_output   = result.error_output
            sub.execution_time = result.execution_time

        db.commit()

    except Exception as e:
        traceback.print_exc()
        try:
            sub.verdict      = Verdict.RE
            sub.error_output = f"Внутренняя ошибка судьи: {str(e)[:500]}"
            db.commit()
        except Exception:
            pass
    finally:
        db.close()
        clear_current_test(submission_id)