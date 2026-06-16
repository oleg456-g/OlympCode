from fastapi import FastAPI, Request
from fastapi.responses import JSONResponse
from fastapi.staticfiles import StaticFiles
from starlette.middleware.sessions import SessionMiddleware

from database import Base, engine, SessionLocal
from models import *  # noqa
from importer import import_all
from routers import auth, tasks, submissions, profile, contests, admin

app = FastAPI(title="OlympCode")

app.add_middleware(SessionMiddleware, secret_key="CHANGE_ME_IN_PRODUCTION")
app.mount("/static", StaticFiles(directory="static"), name="static")

app.include_router(auth.router)
app.include_router(tasks.router)
app.include_router(submissions.router)
app.include_router(profile.router)
app.include_router(contests.router)
app.include_router(admin.router)


@app.on_event("startup")
async def startup():
    Base.metadata.create_all(bind=engine)
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