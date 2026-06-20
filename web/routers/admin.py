"""
Админ-панель: только для пользователей с is_admin=True.
- Создание и редактирование олимпиад
- Загрузка Polygon ZIP и привязка к олимпиаде
"""

import asyncio
import os
import shutil
import tempfile
from pathlib import Path

from fastapi import APIRouter, Request, Form, UploadFile, File, Depends
from fastapi.responses import RedirectResponse, JSONResponse, StreamingResponse
from fastapi.templating import Jinja2Templates
from sqlalchemy.orm import Session

from database import get_db
from models import Contest, ContestTask, Task, Submission, TestResult, Subtask, ContestFormat
from routers.auth import get_current_user
from polygon_importer import import_polygon_zip, CONTESTS_DATA_DIR
import import_worker
from mosh_importer import import_mosh_contest_zip, import_mosh_task_zip
from routers.utils import render_404, templates


_contests_data = Path(__file__).parent.parent.parent / "contests_data"

router = APIRouter(prefix="/admin", tags=["admin"])
templates = Jinja2Templates(directory="templates")


def _require_admin(request: Request, db: Session):
    user = get_current_user(request, db)
    if not user or not user.is_admin:
        return None
    return user


# ── Главная страница админки ──────────────────────────────────────────────────

@router.get("/")
async def admin_index(request: Request, db: Session = Depends(get_db)):
    user = _require_admin(request, db)
    if not user:
        return render_404(request, None)

    contests = db.query(Contest).order_by(Contest.year.desc(), Contest.title).all()
    tasks_count = db.query(Task).count()

    return templates.TemplateResponse("admin/index.html", {
        "request":      request,
        "user":         user,
        "contests":     contests,
        "tasks_count":  tasks_count,
    })


# ── Создание олимпиады ────────────────────────────────────────────────────────

@router.post("/contest/create")
async def create_contest(
    request: Request,
    title:       str = Form(...),
    year:        str = Form(default=""),
    description: str = Form(default=""),
    contest_format: str = Form(default="polygon"),
    db: Session      = Depends(get_db),
):
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "Нет доступа"}, status_code=403)

    fmt = ContestFormat.MOSH if contest_format == "mosh" else ContestFormat.POLYGON
    contest = Contest(
        title       = title,
        year        = int(year) if year.strip().isdigit() else None,
        description = description or None,
        format      = fmt,
    )
    db.add(contest)
    db.commit()
    db.refresh(contest)
    return RedirectResponse(f"/admin/contest/{contest.id}", status_code=303)


# ── Страница олимпиады в админке ──────────────────────────────────────────────

@router.get("/contest/{contest_id}")
async def admin_contest(contest_id: int, request: Request, db: Session = Depends(get_db)):
    user = _require_admin(request, db)
    if not user:
        return render_404(request, None)

    contest = db.query(Contest).filter_by(id=contest_id).first()
    if not contest:
        return render_404(request, user)

    contest_tasks = (
        db.query(ContestTask)
        .filter_by(contest_id=contest_id)
        .order_by(ContestTask.order)
        .all()
    )

    return templates.TemplateResponse("admin/contest.html", {
        "request":       request,
        "user":          user,
        "contest":       contest,
        "contest_tasks": contest_tasks,
    })


# ── Редактирование олимпиады ──────────────────────────────────────────────────

@router.post("/contest/{contest_id}/update")
async def update_contest(
    contest_id:  int,
    request:     Request,
    title:       str = Form(...),
    year:        str = Form(default=""),
    description: str = Form(default=""),
    db: Session       = Depends(get_db),
):
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "Нет доступа"}, status_code=403)

    contest = db.query(Contest).filter_by(id=contest_id).first()
    if not contest:
        return JSONResponse({"error": "Олимпиада не найдена"}, status_code=404)

    contest.title       = title.strip()
    contest.year        = int(year) if year.strip().isdigit() else None
    contest.description = description.strip() or None
    db.commit()
    return RedirectResponse(f"/admin/contest/{contest_id}?ok=Олимпиада+обновлена", status_code=303)


# ── Изменение порядка задач ───────────────────────────────────────────────────

@router.post("/contest/{contest_id}/reorder")
async def reorder_contest_tasks(
    contest_id: int,
    request:    Request,
    order:      str = Form(...),
    db: Session     = Depends(get_db),
):
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "Нет доступа"}, status_code=403)

    contest = db.query(Contest).filter_by(id=contest_id).first()
    if not contest:
        return JSONResponse({"error": "Олимпиада не найдена"}, status_code=404)

    try:
        ct_ids = [int(x) for x in order.split(",") if x.strip()]
    except ValueError:
        return RedirectResponse(f"/admin/contest/{contest_id}?error=Неверный+формат+порядка", status_code=303)

    contest_tasks = {
        ct.id: ct
        for ct in db.query(ContestTask).filter_by(contest_id=contest_id).all()
    }
    if set(ct_ids) != set(contest_tasks.keys()):
        return RedirectResponse(f"/admin/contest/{contest_id}?error=Неполный+список+задач", status_code=303)

    for idx, ct_id in enumerate(ct_ids, start=1):
        ct = contest_tasks[ct_id]
        ct.order = idx
        if contest.format != ContestFormat.MOSH:
            ct.letter = chr(ord("A") + idx - 1)
        if ct.task:
            ct.task.order_in_topic = idx

    db.commit()
    return RedirectResponse(f"/admin/contest/{contest_id}?ok=Порядок+задач+обновлён", status_code=303)


# ── Загрузка Polygon ZIP ──────────────────────────────────────────────────────

@router.post("/contest/{contest_id}/upload")
async def upload_task(
    contest_id:   int,
    request:      Request,
    file:         UploadFile = File(...),
    task_name:    str        = Form(default=""),
    letter:       str        = Form(default=""),
    scoring_type: str        = Form(default="icpc"),
    db: Session              = Depends(get_db),
):
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "Нет доступа"}, status_code=403)

    contest = db.query(Contest).filter_by(id=contest_id).first()
    if not contest:
        return JSONResponse({"error": "Олимпиада не найдена"}, status_code=404)

    if not file.filename.endswith(".zip"):
        return JSONResponse({"error": "Только .zip файлы"}, status_code=400)

    with tempfile.NamedTemporaryFile(suffix=".zip", delete=False) as tmp:
        tmp.write(await file.read())
        tmp_path = tmp.name

    job = import_worker.start_polygon_import(
        tmp_path     = tmp_path,
        contest_id   = contest_id,
        letter       = letter.strip(),
        task_name    = task_name.strip(),
        scoring_type = scoring_type,
        filename     = file.filename,
    )
    return JSONResponse({"job_id": job.job_id})


@router.post("/contest/{contest_id}/upload-mosh")
async def upload_mosh(
    contest_id: int,
    request:    Request,
    file:       UploadFile = File(...),
    task_name:  str        = Form(default=""),
    letter:     str        = Form(default=""),
    db: Session            = Depends(get_db),
):
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "Нет доступа"}, status_code=403)

    contest = db.query(Contest).filter_by(id=contest_id).first()
    if not contest:
        return JSONResponse({"error": "Олимпиада не найдена"}, status_code=404)

    if not file.filename.endswith(".zip"):
        return JSONResponse({"error": "Только .zip файлы"}, status_code=400)

    with tempfile.NamedTemporaryFile(suffix=".zip", delete=False) as tmp:
        tmp.write(await file.read())
        tmp_path = tmp.name

    job = import_worker.start_mosh_import(
        tmp_path   = tmp_path,
        contest_id = contest_id,
        letter     = letter.strip(),
        task_name  = task_name.strip(),
        filename   = file.filename,
    )
    return JSONResponse({"job_id": job.job_id})


# ── SSE: прогресс импорта ────────────────────────────────────────────────────

@router.get("/contest/{contest_id}/import-progress/{job_id}")
async def import_progress_sse(contest_id: int, job_id: str, request: Request):
    """SSE-стрим прогресса импорта. Шлёт обновления каждые 300мс до завершения."""
    import json as _json

    async def event_stream():
        while True:
            if await request.is_disconnected():
                break

            job = import_worker.get_job(job_id)
            if job is None:
                yield f"data: {_json.dumps({'status': 'error', 'message': 'Джоб не найден'})}\n\n"
                break

            yield f"data: {_json.dumps(job.to_dict())}\n\n"

            if job.status in (import_worker.JobStatus.DONE, import_worker.JobStatus.ERROR):
                break

            await asyncio.sleep(0.3)

    return StreamingResponse(
        event_stream(),
        media_type="text/event-stream",
        headers={
            "Cache-Control": "no-cache",
            "X-Accel-Buffering": "no",
        },
    )


@router.get("/contest/{contest_id}/import-jobs")
async def import_jobs(contest_id: int, request: Request, db: Session = Depends(get_db)):
    """Возвращает список активных джобов для контеста (для восстановления состояния при перезагрузке)."""
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "Нет доступа"}, status_code=403)

    jobs = import_worker.get_contest_jobs(contest_id)
    return JSONResponse([j.to_dict() for j in jobs])


# ── Полное удаление задачи ────────────────────────────────────────────────────

def _task_data_dir(task: Task) -> Path | None:
    if not task.tests_path:
        return None
    return Path(task.tests_path).parent


def _delete_task_files(task: Task) -> None:
    data_dir = _task_data_dir(task)
    if not data_dir or not data_dir.exists():
        return
    try:
        data_dir.resolve().relative_to(CONTESTS_DATA_DIR.resolve())
    except ValueError:
        return
    shutil.rmtree(data_dir, ignore_errors=True)

def _delete_task_from_db(task: Task, db: Session) -> None:
    submission_ids = [s.id for s in db.query(Submission).filter_by(task_id=task.id).all()]
    if submission_ids:
        db.query(TestResult).filter(TestResult.submission_id.in_(submission_ids)).delete(
            synchronize_session=False,
        )
    db.query(Submission).filter_by(task_id=task.id).delete(synchronize_session=False)
    db.query(Subtask).filter_by(task_id=task.id).delete(synchronize_session=False)
    db.query(ContestTask).filter_by(task_id=task.id).delete(synchronize_session=False)
    db.delete(task)


@router.post("/contest/{contest_id}/delete/{ct_id}")
async def delete_contest_task(
    contest_id: int,
    ct_id:      int,
    request:    Request,
    db: Session = Depends(get_db),
):
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "Нет доступа"}, status_code=403)

    ct = db.query(ContestTask).filter_by(id=ct_id, contest_id=contest_id).first()
    if not ct:
        return RedirectResponse(f"/admin/contest/{contest_id}", status_code=303)

    task = ct.task
    if task:
        _delete_task_files(task)
        _delete_task_from_db(task, db)
    else:
        db.delete(ct)

    db.commit()
    return RedirectResponse(f"/admin/contest/{contest_id}?ok=Задача+удалена", status_code=303)


# ── Полное удаление контеста ──────────────────────────────────────────────────

def _contest_data_dir(contest: Contest) -> Path | None:
    if not contest or not contest.id:
        return None
    contest_dir = _contests_data / str(contest.id)

    return contest_dir.resolve()


def _delete_contest_files(contest: Contest) -> None:
    data_dir = _contest_data_dir(contest)
    if not data_dir or not data_dir.exists():
        return
    
    try:
        data_dir.relative_to(_contests_data.resolve())
    except ValueError:
        return
    shutil.rmtree(data_dir, ignore_errors=True)


def _delete_contest_from_db(contest: Contest, db: Session) -> None:
    db.delete(contest)


@router.post("/contest/delete/{contest_id}")
async def delete_contest(
    contest_id: int,
    request:    Request,
    db: Session = Depends(get_db),
):
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "Нет доступа"}, status_code=403)

    contest = db.query(Contest).filter_by(id=contest_id).first()
    if not contest:
        return RedirectResponse(f"/admin", status_code=303)
    
    tasks = db.query(ContestTask).filter_by(contest_id=contest_id).all()
    for ct in tasks:
        actual_task = ct.task
        if actual_task:
            _delete_task_files(actual_task)
            _delete_task_from_db(actual_task, db)
            
        db.delete(ct)

    _delete_contest_files(contest)
    _delete_contest_from_db(contest, db)
    db.commit()

    return RedirectResponse(f"/admin/?ok=Контест+удален", status_code=303)