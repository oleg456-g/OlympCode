import json
from datetime import datetime
from fastapi import APIRouter, Request, Form, Depends, BackgroundTasks
from fastapi.responses import JSONResponse
from sqlalchemy.orm import Session

from database import get_db
from models import Submission, Task, Verdict, ScoringType, TestResult
from routers.auth import require_user, get_current_user
from routers.utils import render_404, templates
from judge.judge import judge
from ioi_judge import judge_ioi
from mosh_judge import judge_mosh
from interactive_judge import judge_interactive

router = APIRouter(prefix="/submissions", tags=["submissions"])


def _next_task_submission_number(db: Session, user_id: int, task_id: int) -> int:
    last = (
        db.query(Submission)
        .filter_by(user_id=user_id, task_id=task_id)
        .order_by(Submission.task_submission_number.desc())
        .first()
    )
    return (last.task_submission_number + 1) if last else 1


def _run_judge(submission_id: int) -> None:
    from database import SessionLocal
    db = SessionLocal()
    try:
        sub  = db.query(Submission).filter_by(id=submission_id).first()
        task = db.query(Task).filter_by(id=sub.task_id).first()

        sub.verdict = Verdict.RUNNING
        db.commit()

        if task.is_interactive and task.interactor_path:
            # Интерактивный тестер — два процесса с relay
            test_scores = json.loads(task.test_scores_json) if task.test_scores_json else []

            result = judge_interactive(
                code             = sub.code,
                language         = sub.language,
                tests_path       = task.tests_path,
                time_limit       = task.time_limit,
                memory_limit     = task.memory_limit,
                interactor_path  = task.interactor_path,
                checker_path     = task.checker_path,
                test_scores      = test_scores,
            )
            sub.verdict        = result.verdict
            sub.score          = result.score
            sub.execution_time = result.execution_time
            sub.error_output   = result.error_output
            for t in result.tests:
                db.add(TestResult(
                    submission_id  = sub.id,
                    test_number    = t.test_number,
                    verdict        = t.verdict,
                    execution_time = t.execution_time,
                    score          = t.score,
                ))

        elif task.scoring_type == ScoringType.MOSH:
            # MOSH-тестер
            test_scores = json.loads(task.test_scores_json) if task.test_scores_json else []

            result = judge_mosh(
                code           = sub.code,
                language       = sub.language,
                tests_path     = task.tests_path,
                time_limit     = task.time_limit,
                memory_limit   = task.memory_limit,
                checker_path   = task.checker_path,
                test_scores    = test_scores,
                is_output_only = task.is_output_only,
            )
            sub.verdict        = result.verdict
            sub.score          = result.score
            sub.execution_time = result.execution_time
            sub.error_output   = result.error_output
            for t in result.tests:
                db.add(TestResult(
                    submission_id  = sub.id,
                    test_number    = t.test_number,
                    verdict        = t.verdict,
                    execution_time = t.execution_time,
                    score          = t.score,
                ))
        elif task.scoring_type in (ScoringType.IOI_SUM, ScoringType.IOI_GROUPS):
            # IOI-тестер
            test_scores = json.loads(task.test_scores_json) if task.test_scores_json else []
            subtasks    = task.subtasks if task.scoring_type == ScoringType.IOI_GROUPS else None

            result = judge_ioi(
                code         = sub.code,
                language     = sub.language,
                tests_path   = task.tests_path,
                time_limit   = task.time_limit,
                memory_limit = task.memory_limit,
                checker_path = task.checker_path,
                scoring_type = task.scoring_type,
                test_scores  = test_scores,
                subtasks     = subtasks,
            )

            sub.verdict        = result.verdict
            sub.score          = result.score
            sub.execution_time = result.execution_time
            sub.error_output   = result.error_output

            # Сохраняем результат по каждому тесту
            for t in result.tests:
                db.add(TestResult(
                    submission_id  = sub.id,
                    test_number    = t.test_number,
                    verdict        = t.verdict,
                    execution_time = t.execution_time,
                    score          = t.score,
                ))
        else:
            # ICPC-тестер
            result = judge(
                code         = sub.code,
                language     = sub.language,
                tests_path   = task.tests_path,
                time_limit   = task.time_limit,
                memory_limit = task.memory_limit,
                checker_path = task.checker_path,
            )
            sub.verdict        = result.verdict
            sub.failed_test    = result.failed_test
            sub.error_output   = result.error_output
            sub.execution_time = result.execution_time

        db.commit()
    finally:
        db.close()


@router.post("/submit/{task_id}")
async def submit(
    task_id: int,
    background_tasks: BackgroundTasks,
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

    background_tasks.add_task(_run_judge, sub.id)

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

    return templates.TemplateResponse("submissions/detail.html", {
        "request": request,
        "user":    user,
        "sub":     sub,
        "task":    sub.task,
        "contest": contest,
    })