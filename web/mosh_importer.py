"""
Импорт задач формата МОШ (meta.json + quitp-чекеры).
Поддерживает архив с несколькими задачами (A1, B2, ...) или одну задачу.
"""

import json
import os
import re
import shutil
import tempfile
import zipfile
from pathlib import Path

from sqlalchemy.orm import Session

from models import Task, ContestTask, ScoringType
from mosh_judge import compute_test_max_scores
from polygon_importer import (
    CONTESTS_DATA_DIR,
    _compile_checker,
    _copy_statement_assets,
    _copy_tests,
    _rewrite_asset_urls,
)

_ASSET_EXTENSIONS = {".png", ".jpg", ".jpeg", ".gif", ".svg", ".webp"}
_TASK_DIR_RE = re.compile(r"^[A-Fa-f]\d+$")


def import_mosh_contest_zip(
    zip_path: str | Path,
    contest_id: int,
    db: Session,
) -> list[Task]:
    """Импортирует все задачи из архива МОШ."""
    zip_path = Path(zip_path)
    imported: list[Task] = []

    with tempfile.TemporaryDirectory() as tmpdir:
        tmpdir = Path(tmpdir)
        with zipfile.ZipFile(zip_path, "r") as zf:
            zf.extractall(tmpdir)

        task_dirs = _find_mosh_task_dirs(tmpdir)
        if not task_dirs:
            raise ValueError("В архиве не найдены задачи МОШ (папки A1, B2, ...)")

        for order, task_dir in enumerate(task_dirs, start=1):
            letter = task_dir.name.upper()
            task = _import_mosh_task_dir(
                task_dir=task_dir,
                contest_id=contest_id,
                letter=letter,
                order=order,
                db=db,
            )
            imported.append(task)

    return imported


def import_mosh_task_zip(
    zip_path: str | Path,
    contest_id: int,
    letter: str,
    task_name: str,
    db: Session,
    progress_cb=None,
) -> Task:
    """Импортирует одну задачу МОШ из ZIP (одна папка или файлы в корне)."""
    zip_path = Path(zip_path)

    def _report(pct, msg):
        if progress_cb:
            progress_cb(pct, msg)

    _report(5, "Распаковка архива МОШ...")

    with tempfile.TemporaryDirectory() as tmpdir:
        tmpdir = Path(tmpdir)
        with zipfile.ZipFile(zip_path, "r") as zf:
            zf.extractall(tmpdir)

        _report(15, "Поиск папки задачи...")

        task_dir = _find_single_task_root(tmpdir)
        if not task_dir:
            raise ValueError("Не найдена папка задачи в архиве")

        return _import_mosh_task_dir(
            task_dir=task_dir,
            contest_id=contest_id,
            letter=letter.upper(),
            order=ord(letter.upper()[0]) - ord("A") + 1 if len(letter) == 1 else 0,
            db=db,
            override_name=task_name.strip() or None,
            progress_cb=progress_cb,
        )


def _find_mosh_task_dirs(tmpdir: Path) -> list[Path]:
    dirs = []
    for d in sorted(tmpdir.iterdir()):
        if d.is_dir() and _TASK_DIR_RE.match(d.name) and (d / "meta.json").exists():
            dirs.append(d)
    return dirs


def _find_single_task_root(tmpdir: Path) -> Path | None:
    if (tmpdir / "meta.json").exists():
        return tmpdir
    subdirs = [d for d in tmpdir.iterdir() if d.is_dir() and (d / "meta.json").exists()]
    if len(subdirs) == 1:
        return subdirs[0]
    if len(subdirs) > 1:
        raise ValueError("В архиве несколько задач — используйте импорт контеста МОШ")
    return None


def _import_mosh_task_dir(
    task_dir: Path,
    contest_id: int,
    letter: str,
    order: int,
    db: Session,
    override_name: str | None = None,
    progress_cb=None,
) -> Task:
    def _report(pct, msg):
        if progress_cb:
            progress_cb(pct, msg)

    _report(20, "Парсинг meta.json...")

    meta = json.loads((task_dir / "meta.json").read_text(encoding="utf-8"))
    pm = meta.get("problemMetadata") or {}
    testSets = pm.get("testSets") or {}
    
    target_names = ["tests", "All tests"]

    all_tests = [
        (test.get("inputPath"), test.get("answerPath"))
        for test_set in testSets if test_set.get("name") in target_names
        for test in test_set.get("matchedTests", [])
    ]

    title = override_name or _title_from_meta(pm) or f"Задача {letter}"
    limits = pm.get("solutionLimits") or {}
    time_limit = float(limits.get("timeLimitMillis", 1000)) / 1000.0
    memory_limit = int(limits.get("memoryLimit", 268435456)) // (1024 * 1024)

    # "TEST_ANSWER" — задача типа output-only: участник сдаёт готовый файл
    # с ответом, а не код программы. "WITH_CHECKER" (или отсутствие поля) —
    # обычная задача с проверкой через checker.
    is_output_only = pm.get("problemTypeMeta") == "TEST_ANSWER"

    _report(30, f"Чтение условия «{title}»...")

    statement_html, statement_dir = _read_mosh_statement(task_dir)

    task_slug = re.sub(r"[^\w\-]", "_", title.lower())[:40]
    dest_dir = CONTESTS_DATA_DIR / str(contest_id) / task_slug
    tests_dst = dest_dir / "tests"
    tests_dst.mkdir(parents=True, exist_ok=True)

    _report(40, "Сохранение условия и ассетов...")

    if statement_html:
        if statement_dir:
            _copy_statement_assets(statement_dir, dest_dir)
        statement_html = _rewrite_asset_urls(statement_html, contest_id, task_slug)
        (dest_dir / "problem.html").write_text(statement_html, encoding="utf-8")

    _report(50, "Копирование тестов...")

    tests_src = task_dir / "tests"
    if tests_src.exists():
        _copy_tests(tests_src, tests_dst)

    _report(60, "Настройка чекера...")

    checker_path = _resolve_mosh_checker(task_dir, dest_dir)

    _report(70, "Подсчёт максимальных баллов по тестам...")

    test_scores = compute_test_max_scores(checker_path, all_tests, tests_dst.as_posix())
    max_score = sum(test_scores)
    test_scores_json = json.dumps(test_scores)

    _report(85, "Сохранение в базу данных...")

    if order <= 0:
        order = _letter_order(letter)

    task = Task(
        topic_id         = None,
        order_in_topic   = order,
        title            = title,
        statement_html   = statement_html,
        statement_md     = None,
        time_limit       = time_limit,
        memory_limit     = memory_limit,
        tests_path       = tests_dst.as_posix(),
        checker_path     = checker_path,
        scoring_type     = ScoringType.MOSH,
        max_score        = max_score,
        test_scores_json = test_scores_json,
        is_output_only   = is_output_only,
    )
    db.add(task)
    db.flush()

    existing = db.query(ContestTask).filter_by(contest_id=contest_id, letter=letter).first()
    if existing:
        existing.task_id = task.id
        existing.order = order
    else:
        db.add(ContestTask(
            contest_id = contest_id,
            task_id    = task.id,
            letter     = letter,
            order      = order,
        ))

    db.commit()

    _report(95, f"Финализация «{title}»...")

    print(f"[mosh] Импортировано: '{title}' → контест {contest_id}, {letter}, max={max_score}")
    return task


def _title_from_meta(pm: dict) -> str:
    names = pm.get("names") or {}
    for key in ("ru", "russian", "en", "english"):
        if names.get(key):
            return names[key]
    for tag in ("problem.xml",):
        pass
    return pm.get("shortName") or ""


def _read_mosh_statement(task_dir: Path) -> tuple[str | None, Path | None]:
    candidates = [
        task_dir / "statements" / ".html" / "russian" / "problem.html",
        task_dir / "statements" / ".html" / "ru" / "full_page.html",
        task_dir / "statements" / "russian" / "problem.html",
        task_dir / "statement" / "problem.html",
    ]
    for path in candidates:
        if not path.exists():
            continue
        try:
            html = path.read_text(encoding="utf-8")
            body = re.search(r"<body[^>]*>(.*?)</body>", html, re.DOTALL | re.IGNORECASE)
            content = body.group(1).strip() if body else html.strip()

            css_path = path.parent / "problem-statement.css"
            if css_path.exists():
                css = css_path.read_text(encoding="utf-8")
                content = f"<style>\n{css}\n</style>\n{content}"
            content = content.replace("background-color: #efefef;", "")
            content = content.replace("background-color: #f2f2f2;", "")
            content = re.sub(r'class="test-example-line[^"]*"', '', content)
            return content, path.parent
        except Exception as e:
            print(f"[mosh] Ошибка чтения условия {path}: {e}")
    return None, None


def _resolve_mosh_checker(task_dir: Path, dest_dir: Path) -> str:
    """Ищет готовый чекер или компилирует check.cpp."""
    dest_dir.mkdir(parents=True, exist_ok=True)

    has_cpp = any(
        (task_dir / rel).exists()
        for rel in ["check.cpp", "files/check.cpp", "checkers/check.cpp"]
    )

    for rel in ["files/check.exe", "checker.exe", "files/check", "checkers/check", "check"]:
        src = task_dir / rel
        if not src.exists() or not src.is_file():
            continue
        if os.name == "nt" and not rel.endswith(".exe") and has_cpp:
            continue  # на Windows компилируем .cpp вместо Linux-бинарника
        dst_name = "checker.exe" if os.name == "nt" else "checker"
        dst = dest_dir / dst_name
        shutil.copy2(src, dst)
        if os.name != "nt":
            dst.chmod(0o755)
        print(f"[mosh] Чекер скопирован: {dst}")
        return dst.as_posix()

    return _compile_checker(task_dir, dest_dir)


def _letter_order(letter: str) -> int:
    m = re.match(r"^([A-Fa-f])(\d+)$", letter)
    if m:
        return (ord(m.group(1).upper()) - ord("A")) * 2 + int(m.group(2))
    if len(letter) == 1:
        return ord(letter.upper()) - ord("A") + 1
    return 1