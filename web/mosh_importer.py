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

from models import Task, ContestTask, Subtask, ScoringType
from mosh_judge import compute_test_max_scores
from polygon_importer import (
    CONTESTS_DATA_DIR,
    _compile_checker,
    _copy_statement_assets,
    _copy_tests,
    _rewrite_asset_urls,
)

_ASSET_EXTENSIONS = {".png", ".jpg", ".jpeg", ".gif", ".svg", ".webp"}
_TASK_DIR_RE = re.compile(r"^[A-Za-z]\d?$")


def import_mosh_contest_zip(
    zip_path: str | Path,
    contest_id: int,
    db: Session,
    progress_cb=None,
) -> list[Task]:
    """Импортирует все задачи из архива МОШ."""
    zip_path = Path(zip_path)
    imported: list[Task] = []

    def _report(pct: int, msg: str):
        if progress_cb:
            progress_cb(pct, msg)

    _report(5, "Распаковка архива МОШ...")

    with tempfile.TemporaryDirectory() as tmpdir:
        tmpdir = Path(tmpdir)
        with zipfile.ZipFile(zip_path, "r") as zf:
            zf.extractall(tmpdir)

        _report(10, "Поиск задач в архиве...")

        task_dirs = _find_mosh_task_dirs(tmpdir)
        if not task_dirs:
            raise ValueError("В архиве не найдены задачи МОШ (папки A1, B2, ...)")

        _report(15, f"Найдено задач: {len(task_dirs)}. Начинаю импорт...")

        total = len(task_dirs)
        for order, task_dir in enumerate(task_dirs, start=1):
            letter = task_dir.name.upper()

            # Прогресс: от 15% до 95% распределяем между задачами
            task_start_pct = 15 + int((order - 1) / total * 80)
            task_end_pct   = 15 + int(order / total * 80)

            def make_task_cb(start, end):
                def cb(pct, msg):
                    # Масштабируем прогресс задачи в общий прогресс
                    scaled = start + int(pct / 100 * (end - start))
                    _report(scaled, f"[{letter}] {msg}")
                return cb

            task = _import_mosh_task_dir(
                task_dir=task_dir,
                contest_id=contest_id,
                letter=letter,
                order=order,
                db=db,
                progress_cb=make_task_cb(task_start_pct, task_end_pct),
            )
            imported.append(task)
            _report(task_end_pct, f"✓ {letter}: {task.title}")

    _report(98, f"Завершение импорта {len(imported)} задач...")
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
    
    target_names = ["samples"]

    all_tests = [
        (test.get("inputPath"), test.get("answerPath"))
        for test_set in testSets if test_set.get("name") not in target_names
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

    _report(60, "Копирование решений авторов...")

    mosh_solutions = pm.get("solutions") or []
    if mosh_solutions:
        solutions_dst = dest_dir / "solutions"
        solutions_dst.mkdir(exist_ok=True)
        used_names: set[str] = set()
        for sol in mosh_solutions:
            src_rel  = sol.get("sourcePath", "")
            verdict  = sol.get("verdict", "unknown").lower()
            if not src_rel:
                continue
            src_path = task_dir / src_rel
            if not src_path.exists():
                # sourcePath может быть относительным без префикса папки
                src_path = task_dir / src_path.name
            if not src_path.exists():
                continue
            # Именуем как verdict + оригинальное имя файла, чтобы было понятно
            safe_verdict = re.sub(r"[^\w\-]", "_", verdict)
            dst_name = f"{safe_verdict}_{src_path.name}"
            counter = 2
            while dst_name in used_names:
                stem, suf = src_path.stem, src_path.suffix
                dst_name = f"{safe_verdict}_{stem}_{counter}{suf}"
                counter += 1
            used_names.add(dst_name)
            shutil.copy2(src_path, solutions_dst / dst_name)

    _report(65, "Настройка чекера...")

    checker_path = _resolve_mosh_checker(task_dir, dest_dir)

    _report(70, "Подсчёт максимальных баллов по тестам...")

    # Новый формат MOSH 2026: testGroupsScoringSettings.json явно задаёт
    # балл за каждую группу тестов. Если файл есть — анализируем его чтобы
    # определить scoring_type и построить Subtask (для IOI_GROUPS).
    # Иначе fallback на вычисление через checker (старый формат = ScoringType.MOSH).
    scoring_file = task_dir / "testGroupsScoringSettings.json"
    subtasks_data = []  # список dict для создания Subtask в БД

    if scoring_file.exists():
        test_scores, scoring_type, subtasks_data = _analyze_scoring_settings(
            scoring_file, testSets
        )
    else:
        test_scores   = compute_test_max_scores(checker_path, all_tests, tests_dst.as_posix())
        scoring_type  = ScoringType.MOSH

    max_score = sum(test_scores)
    # Для IOI_GROUPS реальный максимум — сумма max_score всех Subtask
    # (test_scores для complete-group групп содержит нули)
    if scoring_type == ScoringType.IOI_GROUPS and subtasks_data:
        max_score = sum(st["max_score"] for st in subtasks_data)
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
        scoring_type     = scoring_type,
        max_score        = max_score,
        test_scores_json = test_scores_json,
        is_output_only   = is_output_only,
    )
    db.add(task)
    db.flush()

    # Создаём Subtask для IOI_GROUPS задач
    if scoring_type == ScoringType.IOI_GROUPS and subtasks_data:
        for st in subtasks_data:
            db.add(Subtask(
                task_id         = task.id,
                number          = st["number"],
                max_score       = st["max_score"],
                test_from       = st["test_from"],
                test_to         = st["test_to"],
                depends_on_json = json.dumps(st["depends_on"]) if st["depends_on"] else None,
            ))

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

    print(f"[mosh] Импортировано: '{title}' → контест {contest_id}, {letter}, scoring={scoring_type}, max={max_score}")
    return task


def _analyze_scoring_settings(
    scoring_file: Path, testSets: list
) -> tuple[list[float], "ScoringType", list[dict]]:
    """
    Читает testGroupsScoringSettings.json и возвращает:
      - test_scores: балл за каждый тест в порядке как они лежат на диске
      - scoring_type: MOSH (each-test для всех) или IOI_GROUPS (есть complete-group)
      - subtasks_data: список dict для создания Subtask (только для IOI_GROUPS)

    Форматы групп в settings:
      full_score (complete-group): баллы только если вся группа прошла
      test_score (each-test): балл за каждый пройденный тест независимо
      
    Зависимости в depends ссылаются на поле name (не testset).
    """
    try:
        settings = json.loads(scoring_file.read_text(encoding="utf-8"))
    except Exception as e:
        print(f"[mosh] Ошибка чтения testGroupsScoringSettings.json: {e}")
        return [], ScoringType.MOSH, []

    # Маппинг testset → настройки
    by_testset: dict[str, dict] = {s["testset"]: s for s in settings}
    # Маппинг name → номер Subtask (для построения зависимостей)
    name_to_number: dict[str, int] = {}

    # Определяем есть ли хотя бы одна complete-group (full_score)
    has_complete_group = any(
        "full_score" in s and s.get("full_score", 0) > 0
        for s in settings
    )

    scoring_type = ScoringType.IOI_GROUPS if has_complete_group else ScoringType.MOSH

    # Строим test_scores и subtasks_data по testSets (в порядке тестов)
    test_scores:   list[float] = []
    subtasks_data: list[dict]  = []
    test_cursor = 1           # текущий номер теста (1-based, по позиции на диске)
    subtask_counter = 0

    # Нумерация тестов: считаем что файлы на диске идут подряд в том же порядке
    # что и testSets из meta.json
    for ts in testSets:
        ts_name    = ts.get("name", "")
        ts_tests   = ts.get("matchedTests", [])
        n_tests    = len(ts_tests)
        s          = by_testset.get(ts_name, {})
        name       = s.get("name", ts_name)  # короткое имя группы (для зависимостей)
        feedback   = s.get("feedback", "")

        test_from  = test_cursor
        test_to    = test_cursor + n_tests - 1
        test_cursor += n_tests

        if "full_score" in s:
            # complete-group: баллы только если вся группа прошла
            full_score = float(s["full_score"])
            # В test_scores ставим 0 для каждого теста — баллы даст Subtask
            test_scores.extend([0.0] * n_tests)

            if full_score > 0:
                subtask_counter += 1
                name_to_number[name] = subtask_counter
                subtasks_data.append({
                    "number":    subtask_counter,
                    "max_score": full_score,
                    "test_from": test_from,
                    "test_to":   test_to,
                    "depends_on_names": s.get("depends", []),
                })
            # full_score == 0 (samples) — просто добавляем нули, Subtask не нужен

        elif "test_score" in s:
            # each-test: балл за каждый тест отдельно
            per_test = float(s["test_score"])
            test_scores.extend([per_test] * n_tests)

            if per_test > 0 and has_complete_group:
                # В смешанном режиме (IOI_GROUPS) each-test группы тоже
                # нужно представить как Subtask — иначе ioi_judge не учтёт
                # зависимости на них
                subtask_counter += 1
                name_to_number[name] = subtask_counter
                subtasks_data.append({
                    "number":    subtask_counter,
                    "max_score": per_test * n_tests,
                    "test_from": test_from,
                    "test_to":   test_to,
                    "depends_on_names": s.get("depends", []),
                })
        else:
            # Группа без явных баллов — нули
            test_scores.extend([0.0] * n_tests)

    # Переводим depends_on_names → depends_on (номера Subtask)
    # depends в settings может содержать int или str — нормализуем к str
    for st in subtasks_data:
        st["depends_on"] = [
            name_to_number[str(dep)]
            for dep in st["depends_on_names"]
            if str(dep) in name_to_number
        ]
        # Для each-test групп в IOI_GROUPS режиме нужно пересчитать max_score
        # через test_scores (уже заполнены) — subtask.max_score уже правильный

    # Если IOI_GROUPS — test_scores должны быть нулями для complete-group тестов
    # (баллы начисляет ioi_judge через Subtask), а для each-test — реальными
    # Это уже выполнено выше

    return test_scores, scoring_type, subtasks_data



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
        task_dir / "statements" / ".html" / "ru" / "problem.html",
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
    """Ищет готовый чекер или компилирует check.cpp. Всегда сохраняет .cpp исходник."""
    dest_dir.mkdir(parents=True, exist_ok=True)
    # Ищем .cpp исходник
    cpp_candidates = [
        task_dir / "check.cpp",
        task_dir / "files" / "check.cpp",
        task_dir / "checkers" / "check.cpp",
    ]
    check_src = next((c for c in cpp_candidates if c.exists()), None)
    # Всегда сохраняем .cpp рядом с бинарником
    if check_src:
        shutil.copy2(check_src, dest_dir / "checker.cpp")

    # Ищем готовый бинарник
    for rel in ["files/check.exe", "checker.exe", "files/check", "checkers/check", "check"]:
        src = task_dir / rel
        if not src.exists() or not src.is_file():
            continue
        # На Windows пропускаем Linux-бинарники если есть .cpp для компиляции
        if os.name == "nt" and not rel.endswith(".exe") and check_src:
            continue
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