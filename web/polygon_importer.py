"""
Импортирует задачу из Polygon ZIP-архива в БД.
"""

import os
import re
import shutil
import subprocess
import tempfile
import zipfile
import xml.etree.ElementTree as ET
from pathlib import Path
from sqlalchemy.orm import Session

from models import Task, ContestTask, Subtask, ScoringType

CONTESTS_DATA_DIR = Path(__file__).parent.parent / "contests_data"

_ASSET_EXTENSIONS = {".png", ".jpg", ".jpeg", ".gif", ".svg", ".webp"}
_IMAGE_SRC_RE = re.compile(
    r'src="([^"]+\.(?:png|jpg|jpeg|gif|svg|webp))"',
    re.IGNORECASE,
)

def import_polygon_zip(
    zip_path: str | Path,
    contest_id: int,
    letter: str,
    task_name: str,
    scoring_type: str,
    subtasks_def: list | None,
    db: Session,
    progress_cb=None,
) -> Task | list[Task]:
    """
    Универсальная функция импорта Polygon ZIP.
    Сохраняет оригинальную сигнатуру для полной совместимости с роутером,
    но автоматически переключается между импортом одной задачи и целого контеста.
    """
    def _report(pct: int, msg: str):
        if progress_cb:
            progress_cb(pct, msg)

    zip_path = Path(zip_path)
    if not zip_path.exists():
        raise FileNotFoundError(f"Архив не найден: {zip_path}")

    _report(5, "Распаковка архива...")

    with tempfile.TemporaryDirectory() as tmpdir:
        tmpdir = Path(tmpdir)
        with zipfile.ZipFile(zip_path, "r") as zf:
            zf.extractall(tmpdir)

        # Ищем все директории задач внутри архива
        task_dirs = _find_polygon_task_dirs(tmpdir)
        
        # Если найдено несколько папок ИЛИ если буква пришла пустой из формы — это контест
        is_contest = len(task_dirs) > 1 or not (letter and letter.strip())

        if is_contest:
            if not task_dirs:
                raise ValueError("В архиве не найдены задачи Polygon (отсутствуют problem.xml или папки tests)")

            _report(15, f"Обнаружен архив контеста. Найдено задач: {len(task_dirs)}.")

            # Пробуем считать порядок и буквы из contest.xml
            contest_info = _parse_contest_xml(tmpdir)
            slug_to_letter = contest_info["slug_to_letter"] if contest_info else {}

            # Упорядочиваем задачи: если contest.xml есть — по нему,
            # иначе по алфавиту имён директорий (старое поведение).
            if slug_to_letter:
                # Строим обратный маппинг letter → task_dir по slug
                letter_to_dir: dict[str, Path] = {}
                unmatched: list[Path] = []
                for task_dir in task_dirs:
                    slug = task_dir.name
                    if slug in slug_to_letter:
                        letter_to_dir[slug_to_letter[slug]] = task_dir
                    else:
                        unmatched.append(task_dir)
                        print(f"[polygon] Папка {slug!r} не найдена в contest.xml, буква будет назначена автоматически")

                # Сортируем по букве (A, B, C, ...), затем дописываем неопознанные
                ordered = [
                    (letter, letter_to_dir[letter])
                    for letter in sorted(letter_to_dir)
                ]
                # Неопознанным папкам назначаем следующие свободные буквы
                used_letters = {l for l, _ in ordered}
                next_letter_idx = 0
                for task_dir in sorted(unmatched, key=lambda d: d.name.lower()):
                    while chr(ord("A") + next_letter_idx) in used_letters:
                        next_letter_idx += 1
                    auto_letter = chr(ord("A") + next_letter_idx)
                    ordered.append((auto_letter, task_dir))
                    used_letters.add(auto_letter)
                    next_letter_idx += 1
            else:
                # contest.xml отсутствует — назначаем буквы по алфавиту
                task_dirs.sort(key=lambda d: d.name.lower())
                ordered = [(chr(ord("A") + idx), d) for idx, d in enumerate(task_dirs)]

            imported_tasks = []
            for idx, (calculated_letter, task_dir) in enumerate(ordered):
                # Пытаемся заранее вытащить имя из XML для красивого лога
                xml_path = task_dir / "problem.xml"
                calculated_name = ""
                if xml_path.exists():
                    try:
                        meta = _parse_problem_xml(xml_path)
                        calculated_name = meta.get("name", "")
                    except Exception:
                        pass

                task = _import_polygon_task_dir_directly(
                    task_root=task_dir,
                    contest_id=contest_id,
                    letter=calculated_letter,
                    task_name=calculated_name,
                    scoring_type=scoring_type,
                    subtasks_def=None,
                    db=db,
                    progress_cb=progress_cb,
                    is_contest=True,
                    contest_idx=idx,
                    total_tasks=len(ordered)
                )
                imported_tasks.append(task)

            _report(100, f"Успешно импортировано задач: {len(imported_tasks)}")
            return imported_tasks

        else:
            # Импорт одиночной задачи (оригинальное поведение)
            _report(15, "Обнаружена одиночная задача. Обработка...")
            task_root = _find_task_root(tmpdir)
            if not task_root:
                raise ValueError("Не найдена корневая папка задачи в архиве")

            return _import_polygon_task_dir_directly(
                task_root=task_root,
                contest_id=contest_id,
                letter=letter.upper().strip(),
                task_name=task_name.strip() if task_name else "",
                scoring_type=scoring_type,
                subtasks_def=subtasks_def,
                db=db,
                progress_cb=progress_cb,
                is_contest=False
            )


def _parse_contest_xml(tmpdir: Path) -> dict | None:
    """
    Ищет contest.xml в корне или на один уровень вглубь и парсит его.
    Возвращает словарь:
      {
        "name": str | None,           # название контеста (из <names>)
        "slug_to_letter": {slug: letter}  # маппинг имени папки задачи → буква
      }
    или None если contest.xml не найден.

    URL в contest.xml заканчивается на slug папки задачи:
      https://polygon.codeforces.com/p.../user/warfare-1917
                                                ^^^^^^^^^^^
    Этот slug совпадает с именем директории задачи в problems/<slug>/.
    """
    candidates = [
        tmpdir / "contest.xml",
        *tmpdir.glob("*/contest.xml"),  # на случай если всё вложено в одну папку
    ]
    contest_xml = next((p for p in candidates if p.exists()), None)
    if not contest_xml:
        return None

    try:
        tree = ET.parse(contest_xml)
        root = tree.getroot()
    except ET.ParseError as e:
        print(f"[polygon] Ошибка парсинга contest.xml: {e}")
        return None

    # Название контеста (берём русское, иначе первое попавшееся)
    contest_name = None
    names = root.findall(".//names/name")
    if names:
        ru = next((n for n in names if n.get("language") == "russian"), None)
        contest_name = (ru or names[0]).get("value")

    # Маппинг slug → letter из <problems><problem index="a" url="...slug"/>
    slug_to_letter: dict[str, str] = {}
    for problem in root.findall(".//problems/problem"):
        index = problem.get("index", "").strip()
        url   = problem.get("url", "").rstrip("/")
        if not index or not url:
            continue
        slug = url.split("/")[-1]
        if slug:
            slug_to_letter[slug] = index.upper()

    print(f"[polygon] contest.xml: название={contest_name!r}, задач={len(slug_to_letter)}")
    return {"name": contest_name, "slug_to_letter": slug_to_letter}


def _find_polygon_task_dirs(tmpdir: Path) -> list[Path]:
    """Рекурсивно находит все корни задач (где есть problem.xml или папка tests)."""
    task_dirs = []
    for root, dirs, files in os.walk(tmpdir):
        current_path = Path(root)
        if (current_path / "problem.xml").exists() or (current_path / "tests").exists():
            task_dirs.append(current_path)
            dirs.clear()  # Не уходим вглубь найденной задачи
    return task_dirs


def _import_polygon_task_dir_directly(
    task_root: Path,
    contest_id: int,
    letter: str,
    task_name: str,
    scoring_type: str,
    subtasks_def: list | None,
    db: Session,
    progress_cb=None,
    is_contest: bool = False,
    contest_idx: int = 0,
    total_tasks: int = 1,
) -> Task:
    """Внутренний метод импорта конкретной папки с задачей (оригинальная логика)."""
    def _report(pct: int, msg: str):
        if progress_cb:
            if is_contest:
                # Красиво масштабируем прогресс-бар под общий контест
                base = 15 + int((contest_idx / total_tasks) * 80)
                share = int((pct / 100) * (80 / total_tasks))
                progress_cb(base + share, f"[{letter}] {msg}")
            else:
                progress_cb(pct, msg)

    _report(20, "Парсинг problem.xml...")

    meta = {"name": "", "time_limit": 1.0, "memory_limit": 256, "tests": [], "solutions": [], "interactor_source_path": "", "groups": []}
    xml_path = task_root / "problem.xml"
    if xml_path.exists():
        meta = _parse_problem_xml(xml_path)

    if task_name:
        meta["name"] = task_name
    elif not meta["name"] or meta["name"] == "Unknown":
        meta["name"] = f"Задача {letter.upper()}"

    _report(30, f"Чтение условия «{meta['name']}»...")

    statement_html, statement_dir = _read_statement(task_root)

    task_slug = re.sub(r"[^\w\-]", "_", meta["name"].lower())[:40]
    dest_dir  = CONTESTS_DATA_DIR / str(contest_id) / task_slug
    tests_dst = dest_dir / "tests"
    tests_dst.mkdir(parents=True, exist_ok=True)

    _report(40, "Сохранение условия и ассетов...")

    if statement_html:
        if statement_dir:
            _copy_statement_assets(statement_dir, dest_dir)
        statement_html = _rewrite_asset_urls(statement_html, contest_id, task_slug)
        problem_html_path = dest_dir / "problem.html"
        problem_html_path.write_text(statement_html, encoding="utf-8")

    _extract_solutions(task_root, dest_dir, meta.get("solutions", []))

    _report(50, "Копирование тестов...")

    tests_src = task_root / "tests"
    if tests_src.exists():
        _copy_tests(tests_src, tests_dst)

    _report(60, "Компиляция чекера...")

    checker_path = _compile_checker(task_root, dest_dir)

    interactor_source_path = meta.get("interactor_source_path", "")
    is_interactive = bool(interactor_source_path)
    interactor_path = ""
    if is_interactive:
        _report(65, "Подготовка interactor...")
        interactor_path = _resolve_interactor(task_root, dest_dir, interactor_source_path)

    _report(80, "Сохранение в базу данных...")

    scoring = ScoringType(scoring_type)
    test_scores = [0.0 if t.get("sample") else t.get("score", 0) for t in meta["tests"]]
    max_score = sum(test_scores) if scoring != ScoringType.ICPC else 0.0

    import json
    test_scores_json = json.dumps(test_scores)

    order = ord(letter.upper()) - ord("A") + 1

    task = Task(
        topic_id        = None,
        order_in_topic  = order,
        title           = meta["name"],
        statement_html  = statement_html,
        statement_md    = None,
        time_limit      = meta["time_limit"],
        memory_limit    = meta["memory_limit"],
        tests_path      = tests_dst.as_posix(),
        checker_path    = checker_path,
        scoring_type    = scoring,
        max_score       = max_score,
        test_scores_json= test_scores_json,
        is_interactive  = is_interactive,
        interactor_path = interactor_path or None,
    )
    db.add(task)
    db.flush()

    if scoring == ScoringType.IOI_GROUPS:
        if subtasks_def:
            # Явно переданные подзадачи (ручной оверрайд) имеют приоритет
            # над тем, что распарсилось из XML.
            for i, sd in enumerate(subtasks_def, 1):
                db.add(Subtask(
                    task_id   = task.id,
                    number    = i,
                    max_score = sd["max_score"],
                    test_from = sd["test_from"],
                    test_to   = sd["test_to"],
                ))
        elif meta.get("groups"):
            # Автоматически строим подзадачи из групп, найденных в
            # problem.xml — подтверждено на реальных архивах: тесты внутри
            # одной группы Polygon всегда идут подряд, поэтому test_from/
            # test_to как непрерывный диапазон корректно описывает группу.
            #
            # Subtask создаём только для групп с points-policy=
            # "complete-group" — это и есть классическая IOI-подзадача
            # (баллы только при прохождении всех тестов группы). Группы
            # с "each-test" не нужны как Subtask: баллы там просто
            # суммируются по тестам, что ScoringType.IOI_SUM и так делает
            # без всякой группировки.
            complete_groups = [
                g for g in meta["groups"]
                if g.get("points_policy") == "complete-group"
            ]
            # Сначала строим полный маппинг "имя группы в XML" -> "номер
            # Subtask", чтобы корректно перевести зависимости даже если
            # зависимая группа объявлена в XML после той, что от неё зависит.
            name_to_number = {
                g["name"]: i for i, g in enumerate(complete_groups, 1)
            }
            for i, g in enumerate(complete_groups, 1):
                depends_on_numbers = [
                    name_to_number[dep_name]
                    for dep_name in g.get("depends_on_names", [])
                    if dep_name in name_to_number
                ]
                db.add(Subtask(
                    task_id         = task.id,
                    number          = i,
                    max_score       = g["score"],
                    test_from       = g["test_from"],
                    test_to         = g["test_to"],
                    depends_on_json = json.dumps(depends_on_numbers) if depends_on_numbers else None,
                ))

    existing = db.query(ContestTask).filter_by(
        contest_id=contest_id, letter=letter.upper()
    ).first()
    if existing:
        existing.task_id = task.id
    else:
        db.add(ContestTask(
            contest_id = contest_id,
            task_id    = task.id,
            letter     = letter.upper(),
            order      = order,
        ))

    db.commit()
    _report(95, f"Финализация «{meta['name']}»...")
    return task

def _find_task_root(tmpdir: Path) -> Path | None:
    if (tmpdir / "tests").exists() or (tmpdir / "problem.xml").exists():
        return tmpdir
    subdirs = [d for d in tmpdir.iterdir() if d.is_dir()]
    if len(subdirs) == 1:
        sub = subdirs[0]
        if (sub / "tests").exists() or (sub / "problem.xml").exists():
            return sub
    return tmpdir


def _parse_problem_xml(xml_path: Path) -> dict:
    try:
        tree = ET.parse(xml_path)
        root = tree.getroot()
    except Exception as e:
        print(f"[polygon] Ошибка парсинга XML: {e}")
        return {"name": "", "time_limit": 1.0, "memory_limit": 256, "tests": [], "solutions": [], "interactor_source_path": "", "groups": []}

    name = ""
    for tag in root.iter("name"):
        val = tag.get("value", "").strip()
        if val:
            name = val
            break
    if not name:
        name = "Unknown"

    time_limit   = 1.0
    memory_limit = 256
    for judging in root.iter("judging"):
        tl = judging.get("time-limit", "")
        ml = judging.get("memory-limit", "")
        if tl.isdigit():
            time_limit = int(tl) / 1000.0
        if ml.isdigit():
            memory_limit = int(ml) // (1024 * 1024)
        break

    # Важно: тесты и группы нужно искать строго внутри judging/testset,
    # а не где попало в документе — иначе .//tests/test зацепит ещё и
    # <validators><validator><testset><tests><test verdict="valid"/>,
    # которые не имеют ничего общего с тестами участника (нет points/sample,
    # но есть verdict) и портят подсчёт количества тестов и баллов.
    testset = root.find(".//judging/testset")

    tests = []
    if testset is not None:
        for i, test in enumerate(testset.findall("./tests/test"), 1):
            score = float(test.get("points", 0))
            sample = test.get("sample", "false").lower() == "true"
            group = test.get("group")
            tests.append({"num": i, "score": score, "sample": sample, "group": group})

    groups = []
    if testset is not None:
        for group in testset.findall("./groups/group"):
            g_name = group.get("name", "")
            # each-test: баллы участника = сумма баллов каждого пройденного
            #   теста независимо — группа здесь чисто организационная метка,
            #   Subtask для неё не нужен, ScoringType.IOI_SUM справится сам.
            # complete-group: баллы (бонус) начисляются только если ВСЕ
            #   тесты группы пройдены — это и есть классическая IOI-подзадача,
            #   для неё explicit points-атрибут на <group> хранит бонус.
            points_policy = group.get("points-policy", "each-test")

            # Тесты этой группы — по совпадению test.group с именем группы.
            group_test_nums = [t["num"] for t in tests if t.get("group") == g_name]

            if not group_test_nums:
                # Группа объявлена, но ни один тест на неё не ссылается —
                # пропускаем, иначе получим Subtask с пустым диапазоном.
                continue

            test_from = min(group_test_nums)
            test_to   = max(group_test_nums)

            if points_policy == "complete-group":
                # Баллы группы — это явный бонус на самой группе, а не
                # сумма баллов тестов (тесты внутри такой группы почти
                # всегда имеют points=0, кроме иногда последнего — это
                # просто артефакт экспорта Polygon, не реальный механизм
                # начисления).
                g_score = float(group.get("points", 0))
            else:
                # each-test (или points-policy не указан вообще) — баллы
                # группы как метаданные не используются судьёй, но считаем
                # как сумму баллов тестов внутри неё для полноты информации.
                g_score = sum(
                    t["score"] for t in tests
                    if t.get("group") == g_name
                )

            # Зависимости группы — список ИМЁН других групп (как они
            # называются в XML), которые должны быть полностью пройдены
            # прежде чем баллы за эту группу засчитываются. Имена, а не
            # порядковые номера Subtask — перевод в номера Subtask
            # происходит позже, в import_polygon_zip, когда уже известно,
            # какой номер получит каждая группа после фильтрации.
            depends_on_names = [
                dep.get("group", "")
                for dep in group.findall("./dependencies/dependency")
                if dep.get("group")
            ]

            groups.append({
                "name": g_name,
                "score": g_score,
                "points_policy": points_policy,
                "test_from": test_from,
                "test_to": test_to,
                "depends_on_names": depends_on_names,
            })

    solutions = []
    for sol in root.findall(".//assets/solutions/solution"):
        source = sol.find("source")
        if source is None:
            continue
        path = source.get("path", "")
        if not path:
            continue
        solutions.append({
            "tag":  sol.get("tag", "main"),
            "path": path,
            "type": source.get("type", ""),
        })

    # Наличие <assets><interactor> однозначно отличает интерактивную задачу
    # от обычной — программа участника общается с интерактором в реальном
    # времени, а не просто читает stdin/пишет stdout один раз.
    interactor_source_path = ""
    interactor_elem = root.find(".//assets/interactor")
    if interactor_elem is not None:
        source = interactor_elem.find("source")
        if source is not None:
            interactor_source_path = source.get("path", "")

    return {
        "name": name,
        "time_limit": time_limit,
        "memory_limit": memory_limit,
        "tests": tests,
        "groups": groups,
        "solutions": solutions,
        "interactor_source_path": interactor_source_path,
    }


def _resolve_interactor(task_root: Path, dest_dir: Path, interactor_source_path: str) -> str:
    """
    Компилирует interactor.cpp из исходника.

    В отличие от чекера, путь к исходнику интерактора у нас точно известен
    из problem.xml — не нужно угадывать по списку стандартных имён.
    Судья пока не умеет запускать интерактор (требует двусторонний pipe
    между двумя процессами — отдельная большая задача); эта функция только
    готовит бинарник на диске для последующей отладки/доработки.

    Готовый .exe из архива Polygon (если есть) НЕ используется на Linux —
    Polygon всегда экспортирует бинарники под Windows (type="exe.win32"),
    они не запустятся на сервере. Используем готовый бинарник только если
    сами работаем на Windows (os.name == "nt").
    """
    if not interactor_source_path:
        return ""

    src_path = task_root / interactor_source_path
    if not src_path.exists():
        print(f"[polygon] Исходник interactor не найден: {interactor_source_path}")
        return ""

    if os.name == "nt":
        for candidate_name in [src_path.stem + ".exe", src_path.stem]:
            ready = src_path.parent / candidate_name
            if ready.exists() and ready.is_file() and ready != src_path:
                dest_dir.mkdir(parents=True, exist_ok=True)
                dst = dest_dir / "interactor.exe"
                shutil.copy2(ready, dst)
                print(f"[polygon] Interactor скопирован (готовый бинарник): {dst}")
                return dst.as_posix()

    testlib_dir = None
    for p in [src_path.parent, task_root, task_root / "files"]:
        if (p / "testlib.h").exists():
            testlib_dir = p
            break

    final_exe_name = "interactor.exe" if os.name == "nt" else "interactor"
    interactor_exe_dst = dest_dir / final_exe_name

    with tempfile.TemporaryDirectory() as tmp_build_dir:
        tmp_build_path = Path(tmp_build_dir)
        temp_exe = tmp_build_path / final_exe_name

        cmd = ["g++", "-O2", "-std=c++17", str(src_path), "-o", str(temp_exe)]
        if testlib_dir:
            cmd += [f"-I{testlib_dir}"]

        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
        except Exception as e:
            print(f"[polygon] Ошибка запуска компилятора для interactor: {e}")
            return ""

        if result.returncode != 0:
            print(f"[polygon] Ошибка компиляции interactor:\n{result.stderr[:500]}")
            return ""

        dest_dir.mkdir(parents=True, exist_ok=True)
        shutil.move(str(temp_exe), str(interactor_exe_dst))

    print(f"[polygon] Interactor успешно скомпилирован: {interactor_exe_dst}")
    return interactor_exe_dst.as_posix()



def _extract_solutions(task_root: Path, dest_dir: Path, solutions: list[dict]) -> None:
    """
    Копирует все решения из problem.xml в dest_dir/solutions/, сохраняя
    исходное расширение файла и проставляя тег (main, wrong-answer, ...)
    в имени, чтобы не перепутать решения друг с другом.

    Если в problem.xml решений не нашлось (старый формат архива без
    <assets><solutions>), fallback на старое поведение — ищем любой .cpp
    прямо в папке solutions/ и кладём как solution.cpp, чтобы не потерять
    совместимость с ранее работавшими архивами.
    """
    if not solutions:
        legacy_dir = task_root / "solutions"
        if legacy_dir.exists():
            legacy_file = next(legacy_dir.glob("*.cpp"), None)
            if legacy_file:
                dest_dir.mkdir(parents=True, exist_ok=True)
                shutil.copy2(legacy_file, dest_dir / "solution.cpp")
                print(f"[polygon] Решение скопировано (legacy, без тега): {legacy_file.name}")
        return

    solutions_dst = dest_dir / "solutions"
    used_names: set[str] = set()

    for sol in solutions:
        src_rel = sol.get("path", "")
        tag = sol.get("tag", "main") or "main"
        if not src_rel:
            continue

        src_path = task_root / src_rel
        if not src_path.exists():
            print(f"[polygon] Решение не найдено на диске: {src_rel}")
            continue

        solutions_dst.mkdir(parents=True, exist_ok=True)

        safe_tag = re.sub(r"[^\w\-]", "_", tag)
        dst_name = f"{safe_tag}{src_path.suffix}"
        # На случай нескольких решений с одинаковым тегом (несколько
        # альтернативных верных решений и т.п.) — не затираем друг друга.
        counter = 2
        while dst_name in used_names:
            dst_name = f"{safe_tag}_{counter}{src_path.suffix}"
            counter += 1
        used_names.add(dst_name)

        shutil.copy2(src_path, solutions_dst / dst_name)
        print(f"[polygon] Решение скопировано: {src_rel} → solutions/{dst_name} (tag={tag})")



def _read_statement(task_root: Path) -> tuple[str | None, Path | None]:
    """Читает HTML условие и возвращает (content, директория с ассетами)."""
    candidates = [
        task_root / "statements" / ".html" / "russian" / "problem.html",
        task_root / "statements" / "russian" / "problem.html",
        task_root / "statement" / "problem.html",
    ]
    for path in candidates:
        if path.exists():
            try:
                html = path.read_text(encoding="utf-8")
                body = re.search(r"<body[^>]*>(.*?)</body>", html, re.DOTALL | re.IGNORECASE)
                content = body.group(1).strip() if body else html

                css_path = path.parent / "problem-statement.css"
                if css_path.exists():
                    css_content = css_path.read_text(encoding="utf-8")
                    content = f"<style>\n{css_content}\n</style>\n{content}"
                content = content.replace("background-color: #efefef;", "")
                content = content.replace("background-color: #f2f2f2;", "")
                content = re.sub(r'class="test-example-line[^"]*"', '', content)
                return content, path.parent
            except Exception as e:
                print(f"[polygon] Ошибка чтения условия {path}: {e}")

    legend_tex = task_root / "statement-sections" / "russian" / "legend.tex"
    if legend_tex.exists():
        try:
            legend = legend_tex.read_text(encoding="utf-8").strip()
            return f"<p>{legend}</p>", None
        except Exception:
            pass

    return None, None


def _copy_statement_assets(src_dir: Path, dest_dir: Path) -> None:
    """Копирует картинки и прочие ассеты из папки с problem.html."""
    dest_dir.mkdir(parents=True, exist_ok=True)
    for f in src_dir.iterdir():
        if f.is_file() and f.suffix.lower() in _ASSET_EXTENSIONS:
            shutil.copy2(f, dest_dir / f.name)
            print(f"[polygon] Ассет скопирован: {f.name}")


def _rewrite_asset_urls(html: str, contest_id: int, task_slug: str) -> str:
    base = f"/contests_data/{contest_id}/{task_slug}"

    def repl(match: re.Match) -> str:
        src = match.group(1)
        if src.startswith(("http://", "https://", "/")):
            return match.group(0)
        return f'src="{base}/{src}"'

    return _IMAGE_SRC_RE.sub(repl, html)


def _copy_tests(tests_src: Path, tests_dst: Path) -> None:
    for f in sorted(tests_src.iterdir()):
        if f.is_dir():
            continue
        name = f.name
        if re.match(r"^\d+$", name):
            shutil.copy2(f, tests_dst / f"{name}.in")
        elif re.match(r"^\d+\.a$", name):
            num = name[:-2]
            shutil.copy2(f, tests_dst / f"{num}.out")
        elif name.endswith(".in") or name.endswith(".out"):
            shutil.copy2(f, tests_dst / name)


def _compile_checker(task_root: Path, dest_dir: Path) -> str:
    # Ищем .cpp исходник — он нужен всегда (для сохранения и для компиляции)
    cpp_candidates = [
        task_root / "files" / "check.cpp",
        task_root / "checkers" / "check.cpp",
        task_root / "check.cpp",
    ]
    check_src = next((c for c in cpp_candidates if c.exists()), None)

    # Ищем готовый бинарник
    bin_candidates = [
        task_root / "checker.exe", task_root / "checker",
        task_root / "check.exe",  task_root / "files" / "check.exe",
        task_root / "files" / "check",
    ]
    ready_bin = next((b for b in bin_candidates if b.exists() and b.is_file()), None)

    dest_dir.mkdir(parents=True, exist_ok=True)

    # Всегда сохраняем .cpp исходник рядом с бинарником
    if check_src:
        shutil.copy2(check_src, dest_dir / "checker.cpp")
        print(f"[polygon] Исходник чекера сохранён: {dest_dir / 'checker.cpp'}")

    final_exe_name = "checker.exe" if os.name == "nt" else "checker"
    checker_exe_dst = dest_dir / final_exe_name

    # На Windows: если есть готовый .exe — используем его
    if os.name == "nt" and ready_bin and ready_bin.suffix == ".exe":
        shutil.copy2(ready_bin, checker_exe_dst)
        print(f"[polygon] Чекер скопирован (готовый .exe): {checker_exe_dst}")
        return checker_exe_dst.as_posix()

    # На Linux или если нет .exe — компилируем из .cpp
    if not check_src:
        # Нет ни исходника ни подходящего бинарника — пробуем скопировать Linux-бинарник
        linux_bin = next((b for b in bin_candidates
                          if b.exists() and b.is_file() and b.suffix != ".exe"), None)
        if linux_bin:
            shutil.copy2(linux_bin, checker_exe_dst)
            checker_exe_dst.chmod(0o755)
            print(f"[polygon] Чекер скопирован (Linux бинарник): {checker_exe_dst}")
            return checker_exe_dst.as_posix()
        print("[polygon] Чекер не найден — будет токенное сравнение")
        return ""

    testlib_dir = None
    for p in [check_src.parent, task_root, task_root / "files", task_root / "checkers"]:
        if (p / "testlib.h").exists():
            testlib_dir = p
            break

    with tempfile.TemporaryDirectory() as tmp_build_dir:
        tmp_build_path = Path(tmp_build_dir)
        temp_exe = tmp_build_path / final_exe_name

        cmd = ["g++", "-O2", "-std=c++17", str(check_src), "-o", str(temp_exe)]
        if testlib_dir:
            cmd += [f"-I{testlib_dir}"]

        result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
        if result.returncode != 0:
            print(f"[polygon] Ошибка компиляции чекера:\n{result.stderr[:500]}")
            return ""

        shutil.move(str(temp_exe), str(checker_exe_dst))

    print(f"[polygon] Чекер успешно скомпилирован: {checker_exe_dst}")
    return checker_exe_dst.as_posix()