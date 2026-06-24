import re
import json
from pathlib import Path

from fastapi import APIRouter, Request, Depends, HTTPException
from fastapi.responses import FileResponse, RedirectResponse
from sqlalchemy.orm import Session
import markdown as md

from database import get_db
from models import Topic, Task, Submission, Verdict, ScoringType, ContestTask
from routers.auth import get_current_user
from routers.utils import render_404, templates

router = APIRouter(tags=["tasks"])

_IMAGE_SRC_RE = re.compile(
    r'src="([^"]+\.(?:png|jpg|jpeg|gif|svg|webp))"',
    re.IGNORECASE,
)
VISIBLE_SUBS = 5

def _rewrite_statement_assets(html: str, task: Task) -> str:
    """Подставляет абсолютные URL для картинок рядом с problem.html."""
    if not html or not task.tests_path:
        return html

    task_dir = Path(task.tests_path).parent
    try:
        parts = task_dir.resolve().parts
        idx = parts.index("contests_data")
        base = "/" + "/".join(parts[idx:])
    except ValueError:
        return html

    def repl(match: re.Match) -> str:
        src = match.group(1)
        if src.startswith(("http://", "https://", "/")):
            return match.group(0)
        return f'src="{base}/{src}"'

    return _IMAGE_SRC_RE.sub(repl, html)


@router.get("/")
async def index(request: Request, db: Session = Depends(get_db)):
    return RedirectResponse("/contests/", status_code=302)
    topics = db.query(Topic).order_by(Topic.title).all()

    topics_data = []
    for topic in topics:
        tasks = db.query(Task).filter_by(topic_id=topic.id).order_by(Task.order_in_topic).all()
        solved = 0
        if user:
            solved_ids = {
                s.task_id for s in db.query(Submission)
                .filter_by(user_id=user.id, verdict=Verdict.AC)
                .all()
            }
            solved = sum(1 for t in tasks if t.id in solved_ids)
        topics_data.append({
            "topic":  topic,
            "tasks":  tasks,
            "total":  len(tasks),
            "solved": solved,
        })

    return templates.TemplateResponse("tasks/index.html", {
        "request": request,
        "user": user,
        "topics_data": topics_data,
    })


@router.get("/topic/{slug}")
async def topic_page(slug: str, request: Request, db: Session = Depends(get_db)):
    user  = get_current_user(request, db)
    topic = db.query(Topic).filter_by(slug=slug).first()
    if not topic:
        return render_404(request, user)

    tasks = db.query(Task).filter_by(topic_id=topic.id).order_by(Task.order_in_topic).all()

    task_statuses: dict[int, str] = {}
    if user:
        user_submissions = (
            db.query(Submission)
            .filter_by(user_id=user.id)
            .order_by(Submission.id.asc())
            .all()
        )
        for s in user_submissions:
            verdict_str = s.verdict.value if hasattr(s.verdict, 'value') else str(s.verdict)
            if task_statuses.get(s.task_id) == "AC":
                continue
            task_statuses[s.task_id] = verdict_str

    return templates.TemplateResponse("tasks/topic.html", {
        "request":       request,
        "user":          user,
        "topic":         topic,
        "tasks":         tasks,
        "task_statuses": task_statuses,
    })


@router.get("/task/{task_id}")
async def task_page(task_id: int, request: Request, contest_id: int | None = None, db: Session = Depends(get_db)):
    user = get_current_user(request, db)
    if not user:
        return RedirectResponse(f"/auth/login?next=/task/{task_id}", status_code=302)
    task = db.query(Task).filter_by(id=task_id).first()
    if not task:
        return render_404(request, user)

    if task.scoring_type == ScoringType.MOSH and task.checker_path:
        test_scores = json.loads(task.test_scores_json) if task.test_scores_json else []
        if sum(test_scores) >= 0 and (
            task.max_score != sum(test_scores)
            or task.test_scores_json != json.dumps(test_scores)
        ):
            task.test_scores_json = json.dumps(test_scores)
            task.max_score = sum(test_scores)
            db.commit()

    contest = None
    contest_task = None
    if contest_id:
        from models import Contest
        contest = db.query(Contest).filter_by(id=contest_id).first()
        if contest:
            contest_task = db.query(ContestTask).filter_by(
                contest_id=contest_id, task_id=task_id
            ).first()

    if task.statement_html:
        statement_html = _rewrite_statement_assets(task.statement_html, task)
    elif task.statement_md:
        statement_html = md.markdown(
            task.statement_md,
            extensions=["tables", "fenced_code", "toc", "nl2br"]
        )
    else:
        statement_html = "<p style=\"color:var(--text-2)\">Условие задачи не загружено.</p>"

    last_submissions = []
    if user:
        last_submissions = (
            db.query(Submission)
            .filter_by(user_id=user.id, task_id=task.id)
            .order_by(Submission.submitted_at.desc())
            .limit(50)
            .all()
        )
    return templates.TemplateResponse("tasks/task.html", {
        "request":          request,
        "user":             user,
        "task":             task,
        "statement_html":   statement_html,
        "last_submissions": last_submissions,
        "contest": contest,
        "contest_task": contest_task,
        "VISIBLE_SUBS": VISIBLE_SUBS,
    })


@router.get("/task/{task_id}/download-input")
async def download_task_input(task_id: int, db: Session = Depends(get_db)):
    """
    Отдаёт входные данные для output-only задачи (MOSH).
    Такие задачи всегда содержат ровно один тест.
    """
    task = db.query(Task).filter_by(id=task_id).first()
    if not task or not task.is_output_only:
        raise HTTPException(status_code=404, detail="Задача не найдена")

    tests_dir = Path(task.tests_path)
    if not tests_dir.exists():
        raise HTTPException(status_code=404, detail="Тесты не найдены")

    candidates = sorted(
        [f for f in tests_dir.iterdir() if f.is_file() and not f.name.endswith((".a", ".out", ".ans"))]
    )
    if not candidates:
        raise HTTPException(status_code=404, detail="Входной файл не найден")

    in_file = candidates[0]
    return FileResponse(
        in_file,
        filename=f"input_{task_id}.txt",
        media_type="text/plain",
    )