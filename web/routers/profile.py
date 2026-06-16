from fastapi import APIRouter, Request, Depends
from fastapi.responses import RedirectResponse
from sqlalchemy.orm import Session

from database import get_db
from models import Submission, Verdict, Task, ScoringType
from routers.auth import get_current_user
from routers.utils import templates

router = APIRouter(prefix="/profile", tags=["profile"])


@router.get("/")
async def profile(request: Request, limit: int = 10, db: Session = Depends(get_db)):
    user = get_current_user(request, db)
    if not user:
        return RedirectResponse("/auth/login", status_code=302)


    # 1. ГЛОБАЛЬНАЯ СТАТИСТИКА (считает по всей истории, а не по лимиту)
    total_submissions = db.query(Submission).filter_by(user_id=user.id).count()

    # Считаем решённые задачи (уникальные task_id)
    # Для ICPC — это AC.
    solved_icpc = db.query(Submission.task_id).join(Task).filter(
        Submission.user_id == user.id,
        Task.scoring_type == ScoringType.ICPC,
        Submission.verdict == Verdict.AC
    ).distinct()

    # Для IOI — балл равен максимальному (защита от float: score >= max_score)
    solved_ioi = db.query(Submission.task_id).join(Task).filter(
        Submission.user_id == user.id,
        Task.scoring_type.in_([ScoringType.IOI_SUM, ScoringType.IOI_GROUPS]),
        Submission.score >= Task.max_score,
        Task.max_score > 0
    ).distinct()

    tasks_solved = solved_icpc.union(solved_ioi).count()

    # Считаем успешные посылки глобально
    accepted_icpc = db.query(Submission).join(Task).filter(
        Submission.user_id == user.id, 
        Task.scoring_type == ScoringType.ICPC, 
        Submission.verdict == Verdict.AC
    ).count()
    accepted_ioi = db.query(Submission).join(Task).filter(
        Submission.user_id == user.id, 
        Task.scoring_type.in_([ScoringType.IOI_SUM, ScoringType.IOI_GROUPS]), 
        Submission.score >= Task.max_score
    ).count()
    accepted_total = accepted_icpc + accepted_ioi

    stats = {
        "total":        total_submissions,
        "accepted":     accepted_total,
        "tasks_solved": tasks_solved,
    }

    # 2. ИСТОРИЯ ПОСЫЛОК
    # .desc() гарантирует, что новые посылки всегда будут сверху!
    submissions = (
        db.query(Submission)
        .filter_by(user_id=user.id)
        .order_by(Submission.submitted_at.desc())
        .limit(limit)
        .all()
    )

    return templates.TemplateResponse("profile/index.html", {
        "request":       request,
        "user":          user,
        "submissions":   submissions,
        "stats":         stats,
        "current_limit": limit,
    })