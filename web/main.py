import os

from fastapi import FastAPI, Request, HTTPException
from fastapi.responses import JSONResponse, FileResponse
from fastapi.staticfiles import StaticFiles
from starlette.middleware.sessions import SessionMiddleware
from pathlib import Path
from dotenv import load_dotenv

from database import Base, engine, SessionLocal
from models import *  # noqa
from importer import import_all
from routers import auth, tasks, submissions, profile, contests, admin

load_dotenv()

app = FastAPI(title="OlympCode")

app.add_middleware(SessionMiddleware, secret_key=os.environ["SESSION_KEY"])
app.mount("/static", StaticFiles(directory="static"), name="static")

_contests_data = Path(__file__).parent.parent / "contests_data"

app.include_router(auth.router)
app.include_router(tasks.router)

@app.get("/", include_in_schema=False)
async def root_redirect():
    from fastapi.responses import RedirectResponse
    return RedirectResponse("/contests/", status_code=302)
app.include_router(submissions.router)
app.include_router(profile.router)
app.include_router(contests.router)
app.include_router(admin.router)


@app.on_event("startup")
async def startup():
    from sqlalchemy import text
    Base.metadata.create_all(bind=engine)
    with engine.connect() as conn:
        # ── Миграции contests ──────────────────────────────────────────────
        cols = conn.execute(text("PRAGMA table_info(contests)")).fetchall()
        col_names = {c[1] for c in cols}
        if "format" not in col_names:
            conn.execute(text(
                "ALTER TABLE contests ADD COLUMN format VARCHAR(16) DEFAULT 'polygon'"
            ))

        # ── Миграции tasks ─────────────────────────────────────────────────
        cols = conn.execute(text("PRAGMA table_info(tasks)")).fetchall()
        col_names = {c[1] for c in cols}

        task_migrations = [
            ("is_output_only",  "INTEGER NOT NULL DEFAULT 0"),
            ("is_interactive",  "INTEGER NOT NULL DEFAULT 0"),
            ("interactor_path", "VARCHAR(512)"),
        ]
        for col, typedef in task_migrations:
            if col not in col_names:
                conn.execute(text(f"ALTER TABLE tasks ADD COLUMN {col} {typedef}"))

        # ── Миграции subtasks ──────────────────────────────────────────────
        cols = conn.execute(text("PRAGMA table_info(subtasks)")).fetchall()
        col_names = {c[1] for c in cols}
        if "depends_on_json" not in col_names:
            conn.execute(text("ALTER TABLE subtasks ADD COLUMN depends_on_json TEXT"))

        conn.commit()

    # Сбрасываем все RUNNING → PENDING: если сервер упал во время тестирования,
    # посылки не зависнут навсегда — воркер подхватит их при следующем старте
    db = SessionLocal()
    try:
        from models import Submission, Verdict
        stuck = db.query(Submission).filter(Submission.verdict == Verdict.RUNNING).all()
        if stuck:
            print(f"[startup] Найдено {len(stuck)} зависших посылок (RUNNING) — сбрасываем в PENDING")
            for s in stuck:
                s.verdict = Verdict.PENDING
            db.commit()
    finally:
        db.close()

    # Запускаем персистентный воркер очереди тестирования
    from judge_worker import start_judge_worker
    start_judge_worker()
    print("[startup] Воркер очереди тестирования запущен")
    db = SessionLocal()
    try:
        import_all(db)
    finally:
        db.close()


@app.get("/admin/reimport")
async def reimport(request: Request):
    """Перечитывает parsing/ и добавляет новые задачи без перезапуска."""
    from routers.auth import get_current_user
    db = SessionLocal()
    try:
        user = get_current_user(request, db)
        if not user or not user.is_admin:
            return JSONResponse({"error": "Нет доступа"}, status_code=403)
        import_all(db)
        return JSONResponse({"ok": True, "message": "Импорт завершён"})
    finally:
        db.close()


@app.get("/contests_data/{file_path:path}")
async def serve_dynamic_image(file_path: str):
    base_dir_str = str(_contests_data.resolve())
    full_path = _contests_data / file_path
    
    try:
        full_path_str = str(full_path.resolve())
    except Exception:
        raise HTTPException(status_code=404, detail="Image not found")

    if not full_path_str.startswith(base_dir_str):
        raise HTTPException(status_code=403, detail="Access denied")
        
    if full_path.exists() and full_path.is_file():
        response = FileResponse(full_path)
        response.headers["Cache-Control"] = "no-store, no-cache, must-revalidate, max-age=0"
        return response
        
    raise HTTPException(status_code=404, detail="Image not found")