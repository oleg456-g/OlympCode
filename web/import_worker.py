"""
Фоновый воркер для импорта задач с отслеживанием прогресса.

Хранит статус импорта в памяти (dict). Фронтенд получает обновления через SSE.
"""

import asyncio
import json
import os
import tempfile
import threading
import time
import traceback
import zipfile
from dataclasses import dataclass, field
from enum import Enum
from pathlib import Path
from typing import Any

from database import SessionLocal


class JobStatus(str, Enum):
    PENDING  = "pending"
    RUNNING  = "running"
    DONE     = "done"
    ERROR    = "error"


@dataclass
class ImportJob:
    job_id: str
    contest_id: int
    filename: str
    status: JobStatus = JobStatus.PENDING
    message: str = "В очереди..."
    progress: int = 0        # 0–100
    task_id: int | None = None
    task_title: str = ""
    error: str = ""
    created_at: float = field(default_factory=time.time)

    # Для мульти-импорта: sub-jobs
    sub_jobs: list["ImportJob"] = field(default_factory=list)
    total_tasks: int = 1

    def to_dict(self) -> dict:
        return {
            "job_id":      self.job_id,
            "contest_id":  self.contest_id,
            "filename":    self.filename,
            "status":      self.status.value,
            "message":     self.message,
            "progress":    self.progress,
            "task_id":     self.task_id,
            "task_title":  self.task_title,
            "error":       self.error,
            "total_tasks": self.total_tasks,
            "sub_jobs":    [s.to_dict() for s in self.sub_jobs],
        }


# ── Глобальное хранилище джобов ──────────────────────────────────────────────

_jobs: dict[str, ImportJob] = {}
_job_counter = 0
_lock = threading.Lock()


def _next_job_id() -> str:
    global _job_counter
    with _lock:
        _job_counter += 1
        return f"import-{_job_counter}-{int(time.time())}"


def get_job(job_id: str) -> ImportJob | None:
    return _jobs.get(job_id)


def get_contest_jobs(contest_id: int) -> list[ImportJob]:
    """Все джобы для конкретного контеста (последние 50)."""
    jobs = [j for j in _jobs.values() if j.contest_id == contest_id]
    jobs.sort(key=lambda j: j.created_at, reverse=True)
    return jobs[:50]


def cleanup_old_jobs(max_age: float = 3600) -> None:
    now = time.time()
    stale = [k for k, v in _jobs.items() if now - v.created_at > max_age]
    for k in stale:
        _jobs.pop(k, None)


# ── Определение типа архива ──────────────────────────────────────────────────

def _detect_multi_zip(zip_path: str) -> list[str] | None:
    """
    Проверяет, содержит ли ZIP вложенные .zip-файлы (пакетный импорт).
    Возвращает список имён вложенных архивов или None если это обычный архив.
    """
    try:
        with zipfile.ZipFile(zip_path, "r") as zf:
            names = zf.namelist()
            inner_zips = [n for n in names if n.lower().endswith(".zip") and "/" not in n]
            if inner_zips:
                return inner_zips

            # Вложенные в одну папку?
            inner_zips = [
                n for n in names
                if n.lower().endswith(".zip") and n.count("/") == 1
            ]
            if inner_zips:
                return inner_zips
    except zipfile.BadZipFile:
        return None
    return None


# ── Запуск импорта Polygon ───────────────────────────────────────────────────

def start_polygon_import(
    tmp_path: str,
    contest_id: int,
    letter: str,
    task_name: str,
    scoring_type: str,
    filename: str,
) -> ImportJob:
    """Ставит задачу импорта в очередь и запускает в фоновом потоке."""
    cleanup_old_jobs()

    inner_zips = _detect_multi_zip(tmp_path)

    if inner_zips:
        # Мульти-импорт
        job = ImportJob(
            job_id=_next_job_id(),
            contest_id=contest_id,
            filename=filename,
            total_tasks=len(inner_zips),
            message=f"Обнаружено {len(inner_zips)} архивов, начинаю импорт...",
        )
        _jobs[job.job_id] = job
        thread = threading.Thread(
            target=_run_multi_polygon_import,
            args=(job, tmp_path, inner_zips, contest_id, scoring_type),
            daemon=True,
        )
        thread.start()
    else:
        # Одиночный импорт
        job = ImportJob(
            job_id=_next_job_id(),
            contest_id=contest_id,
            filename=filename,
            total_tasks=1,
        )
        _jobs[job.job_id] = job
        thread = threading.Thread(
            target=_run_single_polygon_import,
            args=(job, tmp_path, contest_id, letter, task_name, scoring_type),
            daemon=True,
        )
        thread.start()

    return job


def _run_single_polygon_import(
    job: ImportJob,
    tmp_path: str,
    contest_id: int,
    letter: str,
    task_name: str,
    scoring_type: str,
) -> None:
    """Импортирует одну задачу Polygon в фоновом потоке."""
    from polygon_importer import import_polygon_zip
    from models import Contest

    db = SessionLocal()
    try:
        job.status = JobStatus.RUNNING
        job.message = "Начинаю импорт..."
        job.progress = 0

        # Проверяем что контест ещё существует
        if not db.query(Contest).filter_by(id=contest_id).first():
            job.status = JobStatus.ERROR
            job.message = "Контест был удалён — импорт отменён"
            return

        def on_progress(pct: int, msg: str):
            job.progress = pct
            job.message = msg

        task = import_polygon_zip(
            zip_path=tmp_path,
            contest_id=contest_id,
            letter=letter,
            task_name=task_name.strip(),
            scoring_type=scoring_type,
            subtasks_def=None,
            db=db,
            progress_cb=on_progress,
        )

        # import_polygon_zip возвращает Task или list[Task]
        if isinstance(task, list):
            job.total_tasks = len(task)
            job.task_id    = task[0].id if task else None
            job.task_title = task[0].title if task else ""
            job.message    = f"Импортировано {len(task)} задач"
        else:
            job.task_id    = task.id
            job.task_title = task.title
            job.message    = f"Успешно: {task.title}"
        job.status   = JobStatus.DONE
        job.progress = 100

    except Exception as e:
        job.status = JobStatus.ERROR
        job.error = str(e)[:500]
        job.message = f"Ошибка: {str(e)[:200]}"
        traceback.print_exc()
    finally:
        db.close()
        _safe_unlink(tmp_path)


def _run_multi_polygon_import(
    job: ImportJob,
    tmp_path: str,
    inner_zips: list[str],
    contest_id: int,
    scoring_type: str,
) -> None:
    """Импортирует несколько задач из мульти-архива."""
    from polygon_importer import import_polygon_zip
    from models import Contest

    db = SessionLocal()
    try:
        job.status = JobStatus.RUNNING

        if not db.query(Contest).filter_by(id=contest_id).first():
            job.status = JobStatus.ERROR
            job.message = "Контест был удалён — импорт отменён"
            return

        with tempfile.TemporaryDirectory() as extract_dir:
            with zipfile.ZipFile(tmp_path, "r") as zf:
                zf.extractall(extract_dir)

            sorted_zips = sorted(inner_zips)

            for idx, zip_name in enumerate(sorted_zips):
                letter = chr(ord("A") + idx)
                sub_job = ImportJob(
                    job_id=f"{job.job_id}-{idx}",
                    contest_id=contest_id,
                    filename=zip_name,
                    message="Начинаю импорт...",
                )
                job.sub_jobs.append(sub_job)

                inner_path = os.path.join(extract_dir, zip_name)
                if not os.path.exists(inner_path):
                    sub_job.status = JobStatus.ERROR
                    sub_job.error = f"Файл {zip_name} не найден в архиве"
                    sub_job.message = f"Ошибка: файл не найден"
                    continue

                try:
                    sub_job.status = JobStatus.RUNNING

                    def make_sub_cb(sj):
                        def cb(pct, msg):
                            sj.progress = pct
                            sj.message = msg
                        return cb

                    # Извлекаем имя задачи из имени файла
                    name_hint = Path(zip_name).stem.replace("-", " ").replace("_", " ")

                    task = import_polygon_zip(
                        zip_path=inner_path,
                        contest_id=contest_id,
                        letter=letter,
                        task_name=name_hint,
                        scoring_type=scoring_type,
                        subtasks_def=None,
                        db=db,
                        progress_cb=make_sub_cb(sub_job),
                    )

                    sub_job.task_id = task.id
                    sub_job.task_title = task.title
                    sub_job.status = JobStatus.DONE
                    sub_job.message = f"Успешно: {task.title}"
                    sub_job.progress = 100

                except Exception as e:
                    sub_job.status = JobStatus.ERROR
                    sub_job.error = str(e)[:500]
                    sub_job.message = f"Ошибка: {str(e)[:200]}"
                    traceback.print_exc()

                # Обновляем общий прогресс
                done = sum(1 for s in job.sub_jobs if s.status in (JobStatus.DONE, JobStatus.ERROR))
                job.progress = int(done / job.total_tasks * 100)
                job.message = f"Импортировано {done}/{job.total_tasks}..."

        # Итог
        done_count = sum(1 for s in job.sub_jobs if s.status == JobStatus.DONE)
        err_count = sum(1 for s in job.sub_jobs if s.status == JobStatus.ERROR)

        if err_count == 0:
            job.status = JobStatus.DONE
            job.message = f"Все {done_count} задач импортированы"
        elif done_count > 0:
            job.status = JobStatus.DONE
            job.message = f"Импортировано {done_count}, ошибок: {err_count}"
        else:
            job.status = JobStatus.ERROR
            job.message = f"Все {err_count} задач с ошибками"

        job.progress = 100

    except Exception as e:
        job.status = JobStatus.ERROR
        job.error = str(e)[:500]
        job.message = f"Ошибка: {str(e)[:200]}"
        traceback.print_exc()
    finally:
        db.close()
        _safe_unlink(tmp_path)


# ── Запуск импорта МОШ ───────────────────────────────────────────────────────

def start_mosh_import(
    tmp_path: str,
    contest_id: int,
    letter: str,
    task_name: str,
    filename: str,
) -> ImportJob:
    """Ставит задачу импорта МОШ в очередь."""
    cleanup_old_jobs()

    job = ImportJob(
        job_id=_next_job_id(),
        contest_id=contest_id,
        filename=filename,
    )
    _jobs[job.job_id] = job

    thread = threading.Thread(
        target=_run_mosh_import,
        args=(job, tmp_path, contest_id, letter, task_name),
        daemon=True,
    )
    thread.start()
    return job


def _run_mosh_import(
    job: ImportJob,
    tmp_path: str,
    contest_id: int,
    letter: str,
    task_name: str,
) -> None:
    from mosh_importer import import_mosh_contest_zip, import_mosh_task_zip
    from models import Contest

    db = SessionLocal()
    try:
        job.status = JobStatus.RUNNING
        job.message = "Начинаю импорт МОШ..."
        job.progress = 0

        # Проверяем что контест ещё существует
        contest = db.query(Contest).filter_by(id=contest_id).first()
        if not contest:
            job.status = JobStatus.ERROR
            job.message = "Контест был удалён — импорт отменён"
            return

        def on_progress(pct: int, msg: str):
            # Проверяем контест на каждом шаге прогресса
            db2 = SessionLocal()
            try:
                if not db2.query(Contest).filter_by(id=contest_id).first():
                    raise RuntimeError("Контест был удалён во время импорта")
            finally:
                db2.close()
            job.progress = pct
            job.message = msg

        if letter.strip():
            task = import_mosh_task_zip(
                zip_path=tmp_path,
                contest_id=contest_id,
                letter=letter.strip(),
                task_name=task_name.strip(),
                db=db,
                progress_cb=on_progress,
            )
            job.task_id = task.id
            job.task_title = task.title
            job.status = JobStatus.DONE
            job.message = f"Успешно: {task.title}"
            job.progress = 100
        else:
            tasks = import_mosh_contest_zip(
                tmp_path,
                contest_id,
                db,
                progress_cb=on_progress,
            )
            job.total_tasks = len(tasks)
            job.status = JobStatus.DONE
            job.message = f"Импортировано {len(tasks)} задач МОШ"
            job.progress = 100

    except RuntimeError as e:
        # Контест удалён во время импорта — не ошибка судьи
        job.status = JobStatus.ERROR
        job.message = str(e)
    except Exception as e:
        job.status = JobStatus.ERROR
        job.error = str(e)[:500]
        job.message = f"Ошибка: {str(e)[:200]}"
        traceback.print_exc()
    finally:
        db.close()
        _safe_unlink(tmp_path)


def _safe_unlink(path: str) -> None:
    try:
        if os.path.exists(path):
            os.unlink(path)
    except OSError:
        pass