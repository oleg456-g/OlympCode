import json
from datetime import datetime
from fastapi import APIRouter, Request, Form, Depends
from fastapi.responses import JSONResponse
from sqlalchemy.orm import Session

from database import get_db
from models import Submission, Task, Verdict, ScoringType, TestResult
from routers.auth import require_user, get_current_user
from routers.utils import render_404, templates

router = APIRouter(prefix="/submissions", tags=["submissions"])


def _next_task_submission_number(db: Session, user_id: int, task_id: int) -> int:
    last = (
        db.query(Submission)
        .filter_by(user_id=user_id, task_id=task_id)
        .order_by(Submission.task_submission_number.desc())
        .first()
    )
    return (last.task_submission_number + 1) if last else 1



@router.post("/submit/{task_id}")
async def submit(
    task_id: int,
    request: Request,
    code:     str = Form(...),
    language: str = Form(default="cpp"),
    db: Session   = Depends(get_db),
):
    user = require_user(request, db)
    task = db.query(Task).filter_by(id=task_id).first()
    if not task:
        return JSONResponse({"error": "Задача не найдена"}, status_code=404)

    sub = Submission(
        user_id                = user.id,
        task_id                = task.id,
        task_submission_number = _next_task_submission_number(db, user.id, task.id),
        code                   = code,
        language               = language,
        verdict                = Verdict.PENDING,
        submitted_at           = datetime.now(),
    )
    db.add(sub)
    db.commit()
    db.refresh(sub)
    # Воркер (judge_worker.py) сам подхватит посылку из очереди PENDING

    return JSONResponse({
        "submission_id":          sub.id,
        "task_submission_number": sub.task_submission_number,
        "verdict":                sub.verdict.value,
    })


@router.get("/task/{task_id}/recent")
async def recent_submissions(task_id: int, request: Request, db: Session = Depends(get_db)):
    user = get_current_user(request, db)
    if not user:
        return JSONResponse([])

    task = db.query(Task).filter_by(id=task_id).first()
    if not task:
        return JSONResponse({"error": "not found"}, status_code=404)

    subs = (
        db.query(Submission)
        .filter_by(user_id=user.id, task_id=task_id)
        .order_by(Submission.submitted_at.desc())
        .limit(50)
        .all()
    )
    return JSONResponse([
        {
            "id":                     s.id,
            "task_submission_number": s.task_submission_number,
            "verdict":                s.verdict.value,
            "language":               s.language,
            "failed_test":            s.failed_test,
            "execution_time":         s.execution_time,
            "score":                  s.score,
            "submitted_at":           s.submitted_at.strftime("%d.%m %H:%M"),
        }
        for s in subs
    ])


@router.get("/status/{submission_id}")
async def submission_status(submission_id: int, request: Request, db: Session = Depends(get_db)):
    from judge_worker import get_current_test
    sub = db.query(Submission).filter_by(id=submission_id).first()
    if not sub:
        return JSONResponse({"error": "not found"}, status_code=404)
    return JSONResponse({
        "verdict":                sub.verdict.value,
        "task_submission_number": sub.task_submission_number,
        "failed_test":            sub.failed_test,
        "score":                  sub.score,
        "max_score":              sub.task.max_score if sub.task else None,
        "error_output":           sub.error_output,
        "execution_time":         sub.execution_time,
        "ai_hint":                sub.ai_hint,
        "scoring_type":           sub.task.scoring_type.value if sub.task else "icpc",
        "current_test":           get_current_test(submission_id),
    })


@router.get("/{submission_id}")
async def submission_detail(submission_id: int, request: Request, contest_id: int | None = None, db: Session = Depends(get_db)):
    user = get_current_user(request, db)
    sub  = db.query(Submission).filter_by(id=submission_id).first()
    if not sub:
        return render_404(request, user)
    if user and sub.user_id != user.id and not user.is_admin:
        return render_404(request, user)
    
    contest = None
    if contest_id:
        from models import Contest
        contest = db.query(Contest).filter_by(id=contest_id).first()

    # Если contest_id не передан — ищем любой контест где есть эта задача
    if not contest and sub.task_id:
        from models import ContestTask, Contest
        ct = db.query(ContestTask).filter_by(task_id=sub.task_id).first()
        if ct:
            contest = db.query(Contest).filter_by(id=ct.contest_id).first()

    return templates.TemplateResponse("submissions/detail.html", {
        "request": request,
        "user":    user,
        "sub":     sub,
        "task":    sub.task,
        "contest": contest,
    })