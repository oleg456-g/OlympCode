import bcrypt
from fastapi import APIRouter, Request, Form, Depends, HTTPException
from fastapi.responses import RedirectResponse
from fastapi.templating import Jinja2Templates
from sqlalchemy.orm import Session

from database import get_db
from models import User

router = APIRouter(prefix="/auth", tags=["auth"])
templates = Jinja2Templates(directory="templates")

# ── Rate limiter (in-memory, без зависимостей) ────────────────────────────────
import time, collections, threading

class _RateLimiter:
    """Скользящее окно: max_calls попыток за window_sec с одного IP."""
    def __init__(self, max_calls: int, window_sec: int):
        self.max_calls  = max_calls
        self.window_sec = window_sec
        self._hits: dict[str, collections.deque] = {}
        self._lock = threading.Lock()

    def is_allowed(self, key: str) -> bool:
        now = time.monotonic()
        with self._lock:
            dq = self._hits.setdefault(key, collections.deque())
            cutoff = now - self.window_sec
            while dq and dq[0] < cutoff:
                dq.popleft()
            if len(dq) >= self.max_calls:
                return False
            dq.append(now)
            return True

# 10 попыток за 60 секунд с одного IP — для логина и регистрации
_auth_limiter = _RateLimiter(max_calls=10, window_sec=60)

def _get_ip(request: Request) -> str:
    # Учитываем X-Forwarded-For если стоит за nginx
    forwarded = request.headers.get("X-Forwarded-For")
    if forwarded:
        return forwarded.split(",")[0].strip()
    return request.client.host if request.client else "unknown"

def _check_rate(request: Request):
    ip = _get_ip(request)
    if not _auth_limiter.is_allowed(ip):
        raise HTTPException(
            status_code=429,
            detail="Слишком много попыток. Подождите минуту."
        )


def _hash(password: str) -> str:
    return bcrypt.hashpw(password.encode()[:72], bcrypt.gensalt()).decode()


def _verify(password: str, hashed: str) -> bool:
    return bcrypt.checkpw(password.encode()[:72], hashed.encode())


# ── Helpers ───────────────────────────────────────────────────────────────────

def get_current_user(request: Request, db: Session = Depends(get_db)) -> User | None:
    user_id = request.session.get("user_id")
    if not user_id:
        return None
    return db.query(User).filter_by(id=user_id).first()


def require_user(request: Request, db: Session = Depends(get_db)) -> User:
    user = get_current_user(request, db)
    if not user:
        raise HTTPException(status_code=401, detail="Требуется авторизация")
    return user


# ── Register ──────────────────────────────────────────────────────────────────

@router.get("/register")
async def register_page(request: Request):
    return templates.TemplateResponse("auth/register.html", {"request": request})


@router.post("/register")
async def register(
    request: Request,
    username: str = Form(...),
    email: str    = Form(...),
    password: str = Form(...),
    db: Session   = Depends(get_db),
):
    _check_rate(request)
    if db.query(User).filter_by(username=username).first():
        return templates.TemplateResponse(
            "auth/register.html",
            {"request": request, "error": "Пользователь с таким именем уже существует"}
        )
    if db.query(User).filter_by(email=email).first():
        return templates.TemplateResponse(
            "auth/register.html",
            {"request": request, "error": "Этот email уже зарегистрирован"}
        )

    user = User(
        username      = username,
        email         = email,
        password_hash = _hash(password),
    )
    db.add(user)
    db.commit()
    db.refresh(user)

    request.session["user_id"] = user.id
    return RedirectResponse("/", status_code=303)


# ── Login ─────────────────────────────────────────────────────────────────────

@router.get("/login")
async def login_page(request: Request):
    return templates.TemplateResponse("auth/login.html", {"request": request})


@router.post("/login")
async def login(
    request: Request,
    username: str = Form(...),
    password: str = Form(...),
    db: Session   = Depends(get_db),
):
    _check_rate(request)
    user = db.query(User).filter_by(username=username).first()
    if not user or not _verify(password, user.password_hash):
        return templates.TemplateResponse(
            "auth/login.html",
            {"request": request, "error": "Неверное имя пользователя или пароль"}
        )

    request.session["user_id"] = user.id
    return RedirectResponse("/", status_code=303)


# ── Logout ────────────────────────────────────────────────────────────────────

@router.get("/logout")
async def logout(request: Request):
    request.session.clear()
    return RedirectResponse("/", status_code=303)