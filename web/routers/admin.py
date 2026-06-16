"""
Админ-панель: только для пользователей с is_admin=True.
- Создание олимпиад
- Загрузка Polygon ZIP и привязка к олимпиаде
"""

import os
import tempfile
from pathlib import Path

from fastapi import APIRouter, Request, Form, UploadFile, File, Depends
from fastapi.responses import RedirectResponse, JSONResponse
from fastapi.templating import Jinja2Templates
from sqlalchemy.orm import Session

from database import get_db
from models import Contest, ContestTask, Task
from routers.auth import get_current_user
from routers.utils import render_404
from polygon_importer import import_polygon_zip

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
    db: Session      = Depends(get_db),
):
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "Нет доступа"}, status_code=403)

    contest = Contest(
        title       = title,
        year        = int(year) if year.strip().isdigit() else None,
        description = description or None,
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


# ── Загрузка Polygon ZIP ──────────────────────────────────────────────────────

@router.post("/contest/{contest_id}/upload")
async def upload_task(
    contest_id:   int,
    request:      Request,
    file:         UploadFile = File(...),
    task_name:    str        = Form(default=""),
    letter:       str        = Form(...),
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

    # Сохраняем временно
    with tempfile.NamedTemporaryFile(suffix=".zip", delete=False) as tmp:
        tmp.write(await file.read())
        tmp_path = tmp.name

    try:
        task = import_polygon_zip(
            zip_path      = tmp_path,
            contest_id    = contest_id,
            letter        = letter,
            task_name     = task_name.strip(),
            scoring_type  = scoring_type,
            subtasks_def  = None,
            db            = db,
        )
        return RedirectResponse(f"/admin/contest/{contest_id}?ok={task.title}", status_code=303)
    except Exception as e:
        return RedirectResponse(f"/admin/contest/{contest_id}?error={str(e)[:200]}", status_code=303)
    finally:
        os.unlink(tmp_path)


# ── Удаление задачи из олимпиады ─────────────────────────────────────────────

@router.post("/contest/{contest_id}/remove/{ct_id}")
async def remove_contest_task(
    contest_id: int,
    ct_id:      int,
    request:    Request,
    db: Session = Depends(get_db),
):
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "Нет доступа"}, status_code=403)

    ct = db.query(ContestTask).filter_by(id=ct_id, contest_id=contest_id).first()
    if ct:
        db.delete(ct)
        db.commit()
    return RedirectResponse(f"/admin/contest/{contest_id}", status_code=303)
