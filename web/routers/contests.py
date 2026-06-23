from fastapi import APIRouter, Request, Depends
from sqlalchemy.orm import Session

from database import get_db
from models import Contest, ContestTask, Submission, Verdict
from routers.auth import get_current_user
import markdown as md
from routers.utils import render_404, templates

router = APIRouter(prefix="/contests", tags=["contests"])


@router.get("/")
async def contests_list(request: Request, db: Session = Depends(get_db)):
    user     = get_current_user(request, db)
    # Обычные пользователи видят только видимые контесты
    if user and getattr(user, 'is_admin', False):
        contests = db.query(Contest).order_by(Contest.year.desc(), Contest.title).all()
    else:
        contests = db.query(Contest).filter_by(is_visible=True).order_by(Contest.year.desc(), Contest.title).all()

    return templates.TemplateResponse("contests/index.html", {
        "request":  request,
        "user":     user,
        "contests": contests,
    })


@router.get("/{contest_id}")
async def contest_detail(contest_id: int, request: Request, db: Session = Depends(get_db)):
    user    = get_current_user(request, db)
    contest = db.query(Contest).filter_by(id=contest_id).first()
    if not contest:
        return render_404(request, user)

    contest_tasks = (
        db.query(ContestTask)
        .filter_by(contest_id=contest_id)
        .order_by(ContestTask.order)
        .all()
    )

    # Статусы задач для пользователя
    task_statuses: dict[int, str] = {}
    task_scores:   dict[int, float] = {}
    if user:
        for ct in contest_tasks:
            subs = (
                db.query(Submission)
                .filter_by(user_id=user.id, task_id=ct.task_id)
                .order_by(Submission.submitted_at.desc())
                .all()
            )
            if not subs:
                continue
            # Лучший результат
            if any(s.verdict == Verdict.AC for s in subs):
                task_statuses[ct.task_id] = "AC"
            else:
                task_statuses[ct.task_id] = subs[0].verdict.value
            # Лучший балл для IOI (берём максимум среди всех посылок)
            scores = [s.score for s in subs if s.score is not None]
            if scores:
                task_scores[ct.task_id] = max(scores)

    return templates.TemplateResponse("contests/detail.html", {
        "request":       request,
        "user":          user,
        "contest":       contest,
        "contest_tasks": contest_tasks,
        "task_statuses": task_statuses,
        "task_scores":   task_scores,
    })