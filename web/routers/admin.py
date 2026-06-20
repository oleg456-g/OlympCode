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


# ── Редактирование задачи ────────────────────────────────────────────────────

@router.get("/task/{task_id}")
async def task_edit_page(task_id: int, request: Request, db: Session = Depends(get_db)):
    user = _require_admin(request, db)
    if not user:
        return RedirectResponse("/auth/login", status_code=302)

    task = db.query(Task).filter_by(id=task_id).first()
    if not task:
        return RedirectResponse("/admin/", status_code=302)

    data_dir = _task_data_dir(task)
    tests_dir = Path(task.tests_path) if task.tests_path else None

    # Список тестов
    tests = []
    if tests_dir and tests_dir.exists():
        in_files = sorted(
            [f for f in tests_dir.iterdir() if f.is_file()
             and not f.name.endswith(('.a', '.ans', '.out'))],
            key=lambda p: int(p.stem) if p.stem.isdigit()
                          else int(p.name) if p.name.isdigit() else 0
        )
        for f in in_files:
            num = int(f.stem) if f.stem.isdigit() else int(f.name) if f.name.isdigit() else 0
            ans = None
            for suf in ('.a', '.ans', '.out'):
                cand = f.parent / (f.name + suf) if not f.suffix else f.with_suffix(suf)
                if cand.exists():
                    ans = cand
                    break
            size = f.stat().st_size
            tests.append({
                "num": num,
                "name": f.name,
                "size": size,
                "has_ans": ans is not None,
                "ans_name": ans.name if ans else None,
            })

    # Список решений
    solutions = []
    solutions_dir = data_dir / "solutions" if data_dir else None
    if solutions_dir and solutions_dir.exists():
        solutions = sorted([
            {"name": f.name, "size": f.stat().st_size}
            for f in solutions_dir.iterdir() if f.is_file()
        ], key=lambda x: x["name"])

    # Файловое дерево задачи
    file_tree = []
    if data_dir and data_dir.exists():
        for item in sorted(data_dir.rglob("*")):
            rel = item.relative_to(data_dir)
            file_tree.append({
                "path":     str(rel).replace("\\", "/"),
                "basename": item.name,
                "is_dir":   item.is_dir(),
                "size":     item.stat().st_size if item.is_file() else 0,
                "depth":    len(rel.parts) - 1,
            })

    checker_name = Path(task.checker_path).name if task.checker_path else ""

    # Все контесты где есть эта задача
    from models import Contest
    contest_tasks = (
        db.query(ContestTask)
        .filter_by(task_id=task_id)
        .all()
    )
    contests = []
    for ct in contest_tasks:
        contest = db.query(Contest).filter_by(id=ct.contest_id).first()
        if contest:
            contests.append({"ct": ct, "contest": contest})

    return templates.TemplateResponse("admin/task_edit.html", {
        "request":      request,
        "user":         user,
        "task":         task,
        "contests":     contests,
        "tests":        tests,
        "solutions":    solutions,
        "file_tree":    file_tree,
        "data_dir":     str(data_dir) if data_dir else "",
        "checker_name": checker_name,
        "ok":           request.query_params.get("ok"),
        "error":        request.query_params.get("error"),
    })


@router.get("/task/{task_id}/test/{test_name}")
async def get_test_content(task_id: int, test_name: str, request: Request,
                           db: Session = Depends(get_db)):
    """Возвращает содержимое теста (input и answer) как JSON."""
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "no access"}, status_code=403)

    task = db.query(Task).filter_by(id=task_id).first()
    if not task:
        return JSONResponse({"error": "not found"}, status_code=404)

    tests_dir = Path(task.tests_path)
    in_file = tests_dir / test_name
    if not in_file.exists() or not in_file.is_file():
        return JSONResponse({"error": "test not found"}, status_code=404)

    # Защита от path traversal
    try:
        in_file.relative_to(tests_dir)
    except ValueError:
        return JSONResponse({"error": "invalid path"}, status_code=400)

    content = in_file.read_text(encoding="utf-8", errors="replace")

    ans_content = ""
    for suf in ('.a', '.ans', '.out'):
        cand = in_file.parent / (in_file.name + suf) if not in_file.suffix else in_file.with_suffix(suf)
        if cand.exists():
            ans_content = cand.read_text(encoding="utf-8", errors="replace")
            break

    return JSONResponse({"input": content, "answer": ans_content})


@router.post("/task/{task_id}/test/{test_name}/save")
async def save_test_content(task_id: int, test_name: str, request: Request,
                             db: Session = Depends(get_db)):
    """Сохраняет изменённый текст теста."""
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "no access"}, status_code=403)

    task = db.query(Task).filter_by(id=task_id).first()
    if not task:
        return JSONResponse({"error": "not found"}, status_code=404)

    tests_dir = Path(task.tests_path)
    in_file = tests_dir / test_name
    try:
        in_file.relative_to(tests_dir)
    except ValueError:
        return JSONResponse({"error": "invalid path"}, status_code=400)

    body = await request.json()
    in_file.write_text(body.get("input", ""), encoding="utf-8")

    ans_text = body.get("answer", "")
    if ans_text.strip():
        # Определяем расширение эталонного ответа
        ans_file = None
        for suf in ('.a', '.ans', '.out'):
            cand = in_file.parent / (in_file.name + suf) if not in_file.suffix else in_file.with_suffix(suf)
            if cand.exists():
                ans_file = cand
                break
        if not ans_file:
            ans_file = in_file.parent / (in_file.name + ".a")
        ans_file.write_text(ans_text, encoding="utf-8")

    return JSONResponse({"ok": True})


@router.post("/task/{task_id}/upload-checker")
async def upload_checker(task_id: int, request: Request,
                         file: UploadFile = File(...),
                         db: Session = Depends(get_db)):
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "no access"}, status_code=403)

    task = db.query(Task).filter_by(id=task_id).first()
    if not task:
        return JSONResponse({"error": "not found"}, status_code=404)

    data_dir = _task_data_dir(task)
    if not data_dir:
        return JSONResponse({"error": "task has no data dir"}, status_code=400)

    ext = Path(file.filename).suffix or ".exe"
    dst = data_dir / f"checker{ext}"
    dst.write_bytes(await file.read())
    if os.name != "nt":
        dst.chmod(0o755)

    task.checker_path = str(dst)
    db.commit()
    return RedirectResponse(f"/admin/task/{task_id}?ok=Checker обновлён", status_code=303)


@router.post("/task/{task_id}/upload-interactor")
async def upload_interactor(task_id: int, request: Request,
                             file: UploadFile = File(...),
                             db: Session = Depends(get_db)):
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "no access"}, status_code=403)

    task = db.query(Task).filter_by(id=task_id).first()
    if not task:
        return JSONResponse({"error": "not found"}, status_code=404)

    data_dir = _task_data_dir(task)
    if not data_dir:
        return JSONResponse({"error": "task has no data dir"}, status_code=400)

    ext = Path(file.filename).suffix or ".exe"
    dst = data_dir / f"interactor{ext}"
    dst.write_bytes(await file.read())
    if os.name != "nt":
        dst.chmod(0o755)

    task.interactor_path = str(dst)
    task.is_interactive  = True
    db.commit()
    return RedirectResponse(f"/admin/task/{task_id}?ok=Interactor обновлён", status_code=303)


@router.post("/task/{task_id}/upload-solution")
async def upload_solution(task_id: int, request: Request,
                           file: UploadFile = File(...),
                           db: Session = Depends(get_db)):
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "no access"}, status_code=403)

    task = db.query(Task).filter_by(id=task_id).first()
    if not task:
        return JSONResponse({"error": "not found"}, status_code=404)

    data_dir = _task_data_dir(task)
    solutions_dir = data_dir / "solutions"
    solutions_dir.mkdir(exist_ok=True)

    dst = solutions_dir / file.filename
    # Антиперезапись
    counter = 2
    while dst.exists():
        stem, suf = Path(file.filename).stem, Path(file.filename).suffix
        dst = solutions_dir / f"{stem}_{counter}{suf}"
        counter += 1

    dst.write_bytes(await file.read())
    return RedirectResponse(f"/admin/task/{task_id}?ok=Решение загружено: {dst.name}", status_code=303)


@router.post("/task/{task_id}/upload-statement")
async def upload_statement(task_id: int, request: Request,
                            file: UploadFile = File(...),
                            db: Session = Depends(get_db)):
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "no access"}, status_code=403)

    task = db.query(Task).filter_by(id=task_id).first()
    if not task:
        return JSONResponse({"error": "not found"}, status_code=404)

    data_dir = _task_data_dir(task)
    dst = data_dir / "problem.html"
    content = await file.read()
    dst.write_bytes(content)

    task.statement_html = content.decode("utf-8", errors="replace")
    db.commit()
    return RedirectResponse(f"/admin/task/{task_id}?ok=Условие обновлено", status_code=303)


@router.post("/task/{task_id}/solution/{filename}/delete")
async def delete_solution(task_id: int, filename: str, request: Request,
                           db: Session = Depends(get_db)):
    user = _require_admin(request, db)
    if not user:
        return JSONResponse({"error": "no access"}, status_code=403)

    task = db.query(Task).filter_by(id=task_id).first()
    if not task:
        return JSONResponse({"error": "not found"}, status_code=404)

    data_dir = _task_data_dir(task)
    target = data_dir / "solutions" / filename
    try:
        target.relative_to(data_dir)  # path traversal guard
        if target.exists():
            target.unlink()
    except ValueError:
        return JSONResponse({"error": "invalid path"}, status_code=400)

    return RedirectResponse(f"/admin/task/{task_id}?ok=Файл удалён", status_code=303)


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