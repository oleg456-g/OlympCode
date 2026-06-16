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

from models import Task, Contest, ContestTask, Subtask, ScoringType

CONTESTS_DATA_DIR = Path(__file__).parent.parent / "contests_data"


def import_polygon_zip(
    zip_path: str | Path,
    contest_id: int,
    letter: str,
    task_name: str,
    scoring_type: str,
    subtasks_def: list | None,
    db: Session,
) -> Task:
    zip_path = Path(zip_path)
    if not zip_path.exists():
        raise FileNotFoundError(f"Архив не найден: {zip_path}")

    with tempfile.TemporaryDirectory() as tmpdir:
        tmpdir = Path(tmpdir)

        with zipfile.ZipFile(zip_path, "r") as zf:
            zf.extractall(tmpdir)

        task_root = _find_task_root(tmpdir)
        if not task_root:
            raise ValueError("Не найдена корневая папка задачи в архиве")

        # Парсим problem.xml если есть
        meta = {"name": task_name, "time_limit": 1.0, "memory_limit": 256, "tests": []}
        xml_path = task_root / "problem.xml"
        if xml_path.exists():
            meta = _parse_problem_xml(xml_path, fallback_name=task_name)

        # Если имя всё ещё пустое — используем то что ввёл пользователь
        if not meta["name"] or meta["name"] == "Unknown":
            meta["name"] = task_name or f"Задача {letter}"

        # Читаем HTML условие
        statement_html = _read_statement(task_root)

        # Копируем тесты
        task_slug = re.sub(r"[^\w\-]", "_", meta["name"].lower())[:40]
        dest_dir  = CONTESTS_DATA_DIR / str(contest_id) / task_slug
        tests_dst = dest_dir / "tests"
        tests_dst.mkdir(parents=True, exist_ok=True)
        
        if statement_html:
            problem_html_path = dest_dir / "problem.html"
            problem_html_path.write_text(statement_html, encoding="utf-8")
            print(f"[polygon] Условие сохранено: {problem_html_path}")

        solutions_dir = task_root / "solutions"
        if solutions_dir.exists():

            solution_file = next(solutions_dir.glob("*.cpp"), None)
            if solution_file:
                dst_solution = dest_dir / "solution.cpp"
                shutil.copy2(solution_file, dst_solution)
                print(f"[polygon] Решение скопировано: {dst_solution}")
        tests_src = task_root / "tests"
        if tests_src.exists():
            _copy_tests(tests_src, tests_dst)

        # Компилируем чекер
        checker_path = _compile_checker(task_root, dest_dir)

        # Scoring
        scoring = ScoringType(scoring_type)
        # Не считаем sample-тесты (points=0) в max_score
        test_scores = [t.get("score", 0) for t in meta["tests"]]
        max_score = sum(test_scores) if scoring != ScoringType.ICPC else 0.0

        # Сохраняем баллы за тесты как JSON-строку для IOI-джаджа
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
        )
        db.add(task)
        db.flush()

        if scoring == ScoringType.IOI_GROUPS and subtasks_def:
            for i, sd in enumerate(subtasks_def, 1):
                db.add(Subtask(
                    task_id   = task.id,
                    number    = i,
                    max_score = sd["max_score"],
                    test_from = sd["test_from"],
                    test_to   = sd["test_to"],
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
        print(f"[polygon] Импортировано: '{meta['name']}' → контест {contest_id}, буква {letter.upper()}")
        return task


def _find_task_root(tmpdir: Path) -> Path | None:
    """Ищет корень задачи — папку с tests/ или problem.xml."""
    # Вариант 1: всё в корне
    if (tmpdir / "tests").exists() or (tmpdir / "problem.xml").exists():
        return tmpdir
    # Вариант 2: одна подпапка
    subdirs = [d for d in tmpdir.iterdir() if d.is_dir()]
    if len(subdirs) == 1:
        sub = subdirs[0]
        if (sub / "tests").exists() or (sub / "problem.xml").exists():
            return sub
    return tmpdir  # fallback


def _parse_problem_xml(xml_path: Path, fallback_name: str = "") -> dict:
    """Парсит problem.xml."""
    try:
        tree = ET.parse(xml_path)
        root = tree.getroot()
    except Exception as e:
        print(f"[polygon] Ошибка парсинга XML: {e}")
        return {"name": fallback_name, "time_limit": 1.0, "memory_limit": 256, "tests": []}

    # Название — пробуем несколько вариантов расположения тега
    name = ""
    for tag in root.iter("name"):
        val = tag.get("value", "").strip()
        if val:
            name = val
            break
    # Если не нашли атрибут value — ищем в statement-sections/russian/name.tex
    if not name:
        name = fallback_name or "Unknown"

    # Лимиты
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

    # Тесты с баллами
    tests = []
    for i, test in enumerate(root.findall(".//tests/test"), 1):
        score = float(test.get("points", 0))
        sample = test.get("sample", "false").lower() == "true"
        tests.append({"num": i, "score": score, "sample": sample})

    # Группы подзадач (если есть тег <groups>)
    groups = []
    for group in root.findall(".//groups/group"):
        g_score = float(group.get("points", 0))
        g_name  = group.get("name", "")
        # Тесты группы через <dependencies> или диапазон
        g_tests = []
        for dep in group.findall("dependencies/dependency"):
            pass  # зависимости между группами, не тесты
        groups.append({"name": g_name, "score": g_score, "tests": g_tests})

    return {
        "name": name,
        "time_limit": time_limit,
        "memory_limit": memory_limit,
        "tests": tests,
        "groups": groups,
    }


def _read_statement(task_root: Path) -> str | None:
    """Читает HTML условие задачи и подтягивает CSS Полигона."""
    candidates = [
        task_root / "statements" / ".html" / "russian" / "problem.html",
        task_root / "statements" / "russian" / "problem.html",
        task_root / "statement" / "problem.html",
    ]
    for path in candidates:
        if path.exists():
            try:
                html = path.read_text(encoding="utf-8")
                # Вытаскиваем содержимое <body>
                body = re.search(r"<body[^>]*>(.*?)</body>", html, re.DOTALL | re.IGNORECASE)
                content = body.group(1).strip() if body else html

                # Ищем родной CSS Полигона рядом с HTML
                css_path = path.parent / "problem-statement.css"
                if css_path.exists():
                    css_content = css_path.read_text(encoding="utf-8")
                    # Встраиваем CSS прямо перед HTML-кодом условия
                    content = f"<style>\n{css_content}\n</style>\n{content}"
                content = content.replace("background-color: #efefef;", "")
                content = content.replace("background-color: #f2f2f2;", "")
                content = re.sub(
                    r'class="test-example-line[^"]*"',
                    '',
                    content
                )
                return content
            except Exception as e:
                print(f"[polygon] Ошибка чтения условия {path}: {e}")
                pass

    # Fallback — читаем name.tex как заголовок
    name_tex = task_root / "statement-sections" / "russian" / "name.tex"
    legend_tex = task_root / "statement-sections" / "russian" / "legend.tex"
    if legend_tex.exists():
        try:
            legend = legend_tex.read_text(encoding="utf-8").strip()
            return f"<p>{legend}</p>"
        except Exception:
            pass

    return None


def _copy_tests(tests_src: Path, tests_dst: Path) -> None:
    """Копирует тесты, поддерживает форматы: 01/01.a и 01.in/01.out."""
    for f in sorted(tests_src.iterdir()):
        if f.is_dir():
            continue
        name = f.name
        if re.match(r"^\d+$", name):
            # Polygon формат: "01" → входные данные
            shutil.copy2(f, tests_dst / f"{name}.in")
        elif re.match(r"^\d+\.a$", name):
            # Polygon формат: "01.a" → ответ
            num = name[:-2]
            shutil.copy2(f, tests_dst / f"{num}.out")
        elif name.endswith(".in") or name.endswith(".out"):
            # Уже наш формат
            shutil.copy2(f, tests_dst / name)


def _compile_checker(task_root: Path, dest_dir: Path) -> str:
    """Компилирует check.cpp, или копирует готовый checker.exe.
    
    Безопасно обрабатывает кириллицу в путях сборки.
    """
    # 1. Ищем уже скомпилированный бинарник
    for name in ["checker.exe", "checker", "check.exe"]:
        ready = task_root / name
        if ready.exists():
            dest_dir.mkdir(parents=True, exist_ok=True)
            dst = dest_dir / name
            shutil.copy2(ready, dst)
            print(f"[polygon] Чекер скопирован: {dst}")
            return dst.as_posix()

    # 2. Ищем исходный код чекера
    candidates = [
        task_root / "files" / "check.cpp",
        task_root / "check.cpp",
    ]
    check_src = next((c for c in candidates if c.exists()), None)
    if not check_src:
        print("[polygon] Чекер не найден — будет токенное сравнение")
        return ""

    # Ищем testlib.h (может быть как в папке с чекером, так и в корне задачи)
    testlib_dir = None
    for p in [check_src.parent, task_root, task_root / "files"]:
        if (p / "testlib.h").exists():
            testlib_dir = p
            break

    # Итоговое имя файла, которое должно получиться в целевой папке
    final_exe_name = "checker.exe" if os.name == "nt" else "checker"
    checker_exe_dst = dest_dir / final_exe_name

    # 3. Компиляция в изолированном временном пространстве без кириллицы
    with tempfile.TemporaryDirectory() as tmp_build_dir:
        tmp_build_path = Path(tmp_build_dir)
        temp_exe = tmp_build_path / final_exe_name

        # Формируем команду компиляции для g++
        cmd = ["g++", "-O2", "-std=c++17", str(check_src), "-o", str(temp_exe)]
        
        if testlib_dir:
            cmd += [f"-I{testlib_dir}"]

        # Запускаем компиляцию
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
        
        if result.returncode != 0:
            print(f"[polygon] Ошибка компиляции чекера:\n{result.stderr[:500]}")
            return ""

        # 4. Переносим скомпилированный файл в целевую директорию
        dest_dir.mkdir(parents=True, exist_ok=True)
        shutil.move(str(temp_exe), str(checker_exe_dst))

    print(f"[polygon] Чекер успешно скомпилирован: {checker_exe_dst}")
    return checker_exe_dst.as_posix()
