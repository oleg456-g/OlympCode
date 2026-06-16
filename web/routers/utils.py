from fastapi import Request
from fastapi.templating import Jinja2Templates

from scoring_utils import format_score

templates = Jinja2Templates(directory="templates")
templates.env.filters["format_score"] = format_score

def render_404(request: Request, user=None):
    return templates.TemplateResponse(
        "404.html", {"request": request, "user": user}, status_code=404
    )