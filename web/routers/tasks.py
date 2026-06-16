from fastapi import APIRouter, Request, Depends
from fastapi.templating import Jinja2Templates
from sqlalchemy.orm import Session
import markdown as md

from database import get_db
from models import Topic, Task, Submission, Verdict
from routers.auth import get_current_user
from routers.utils import render_404

router = APIRouter(tags=["tasks"])
templates = Jinja2Templates(directory="templates")


@router.get("/")
async def index(request: Request, db: Session = Depends(get_db)):
    user   = get_current_user(request, db)
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
    task = db.query(Task).filter_by(id=task_id).first()
    if not task:
        return render_404(request, user)

    contest = None
    if contest_id:
        from models import Contest
        contest = db.query(Contest).filter_by(id=contest_id).first()

    if task.statement_html:
        statement_html = task.statement_html
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
    })