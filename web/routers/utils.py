from fastapi import Request
from fastapi.templating import Jinja2Templates

templates = Jinja2Templates(directory="templates")

def render_404(request: Request, user=None):
    return templates.TemplateResponse(
        "404.html", {"request": request, "user": user}, status_code=404
    )