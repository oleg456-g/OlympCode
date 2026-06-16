"""
Читает структуру parsing/ и заполняет БД задачами.
Запускается один раз при старте приложения (идемпотентно).

Ожидаемая структура:
  parsing/
    <topic-slug>/        # например "binary search", "sorting"
      <N>/               # номер задачи: 1, 2, 3, ...
        statement.md
        tests/
          1.in  1.out  2.in  2.out ...
"""

import os
import re
from pathlib import Path

from sqlalchemy.orm import Session
from models import Topic, Task, CheckerType


PARSING_DIR = Path(__file__).parent.parent / "parsing"

# Slug = имя папки в нижнем регистре с пробелами → дефис
def _slug(name: str) -> str:
    return re.sub(r"[\s_]+", "-", name.strip().lower())

def _title(name: str) -> str:
    # "binary search" → "Binary Search", "binary_search" → "Binary Search"
    return re.sub(r"[\s_]+", " ", name.strip()).title()



def _parse_frontmatter(text: str) -> tuple[dict, str]:
    """Вытаскивает YAML-фронтматтер вида ---\nkey: val\n--- из markdown."""
    meta: dict = {}
    body = text
    if text.startswith("---"):
        end = text.find("---", 3)
        if end != -1:
            fm_block = text[3:end].strip()
            for line in fm_block.splitlines():
                if ":" in line:
                    k, _, v = line.partition(":")
                    meta[k.strip()] = v.strip()
            body = text[end + 3:].strip()
    return meta, body


def get_default_checker_path(checker_type: CheckerType, topic_slug: str, order_in_topic: int) -> Path:
    """
    Возвращает относительный Path к чекеру на основе его типа и структуры проекта.
    """
    base_checkers_dir = Path(__file__).parent.parent / "web" / "judge" / "checkers"
    if checker_type == CheckerType.NCMP:
        return base_checkers_dir / "ncmp.exe"
    elif checker_type == CheckerType.WCMP:
        return base_checkers_dir / "wcmp.exe"
    elif checker_type == CheckerType.RCMP4:
        return base_checkers_dir / "rcmp4.exe"    
    elif checker_type == CheckerType.RCMP6:
        return base_checkers_dir / "rcmp6.exe"
    elif checker_type == CheckerType.CUSTOM:
        # web/judge/checkers/custom/{topic_slug}/{order_in_topic}.exe
        return base_checkers_dir / "custom" / topic_slug / f"{order_in_topic}.exe"
    else:
        raise ValueError(f"Неизвестный тип чекера: {checker_type}")


def import_all(db: Session) -> None:
    if not PARSING_DIR.exists():
        print(f"[importer] Папка {PARSING_DIR} не найдена, пропускаем импорт.")
        return

    for topic_dir in sorted(PARSING_DIR.iterdir()):
        if not topic_dir.is_dir():
            continue

        slug  = _slug(topic_dir.name)
        title = _title(topic_dir.name)

        topic = db.query(Topic).filter_by(slug=slug).first()
        if not topic:
            topic = Topic(slug=slug, title=title)
            db.add(topic)
            db.flush()
            print(f"[importer] Тема добавлена: {title}")

        for task_dir in sorted(topic_dir.iterdir(), key=lambda p: int(p.name) if p.name.isdigit() else 0):
            if not task_dir.is_dir() or not task_dir.name.isdigit():
                continue

            order = int(task_dir.name)
            stmt_file = task_dir / "statement.md"
            if not stmt_file.exists():
                continue

            tests_path = task_dir / "tests"

            # Уже импортировано?
            existing = db.query(Task).filter_by(
                topic_id=topic.id, order_in_topic=order
            ).first()
            if existing:
                continue

            raw = stmt_file.read_text(encoding="utf-8")
            meta, body = _parse_frontmatter(raw)

            task_title    = meta.get("title", f"{title} #{order}")
            time_limit    = float(meta.get("time_limit", 1.0))
            memory_limit  = int(meta.get("memory_limit", 256))
            raw_checker_type  = meta.get("checker_type", "wcmp").strip().lower()
            
            checker_type = CheckerType(raw_checker_type)
            checker_path = get_default_checker_path(checker_type, slug, order)


            task = Task(
                topic_id      = topic.id,
                order_in_topic= order,
                title         = task_title,
                statement_md  = body,
                time_limit    = time_limit,
                memory_limit  = memory_limit,
                tests_path    = tests_path.as_posix(),
                checker_path  = checker_path.as_posix(),
            )
            db.add(task)
            print(f"[importer]   Задача добавлена: {title} #{order} — {task_title}")

    db.commit()
    print("[importer] Импорт завершён.")