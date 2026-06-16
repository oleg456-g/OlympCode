"""Утилиты для баллов: примеры из условия, форматирование."""

import re
from pathlib import Path


def format_score(value: float | int | None) -> str:
    if value is None:
        return "0"
    value = float(value)
    if abs(value - round(value)) < 1e-9:
        return str(int(round(value)))
    text = f"{value:.2f}".rstrip("0").rstrip(".")
    return text or "0"


def normalize_test_input(text: str) -> str:
    lines = text.replace("\r\n", "\n").replace("\r", "\n").split("\n")
    while lines and not lines[-1].strip():
        lines.pop()
    while lines and not lines[0].strip():
        lines.pop(0)
    return "\n".join(line.rstrip() for line in lines)


def extract_sample_inputs(statement_html: str) -> list[str]:
    if not statement_html:
        return []

    samples: list[str] = []
    seen: set[str] = set()

    patterns = [
        r'class="sample-test"[^>]*>.*?class="input"[^>]*>.*?<PRE[^>]*>(.*?)</PRE>',
        r'class="sample-tests"[^>]*>.*?class="input"[^>]*>.*?<PRE[^>]*>(.*?)</PRE>',
        r'class="sample-test"[^>]*>.*?<PRE[^>]*class="[^"]*content[^"]*"[^>]*>(.*?)</PRE>',
    ]
    for pattern in patterns:
        for match in re.finditer(pattern, statement_html, re.DOTALL | re.IGNORECASE):
            normalized = normalize_test_input(match.group(1))
            if normalized and normalized not in seen:
                seen.add(normalized)
                samples.append(normalized)

    return samples


def _list_input_files(tests_dir: Path) -> list[Path]:
    in_files = sorted(
        tests_dir.glob("*.in"),
        key=lambda p: int(p.stem) if p.stem.isdigit() else 0,
    )
    if in_files:
        return in_files
    return sorted(
        (f for f in tests_dir.iterdir() if f.is_file() and f.name.isdigit()),
        key=lambda p: int(p.name),
    )


def detect_sample_test_numbers(statement_html: str, tests_path: str) -> set[int]:
    sample_inputs = extract_sample_inputs(statement_html)
    if not sample_inputs:
        return set()

    tests_dir = Path(tests_path)
    if not tests_dir.exists():
        return set()

    sample_nums: set[int] = set()
    for in_file in _list_input_files(tests_dir):
        try:
            content = normalize_test_input(in_file.read_text(encoding="utf-8"))
        except OSError:
            continue
        if content in sample_inputs:
            num = int(in_file.stem) if in_file.stem.isdigit() else int(in_file.name)
            sample_nums.add(num)
    return sample_nums


def zero_out_sample_tests(
    test_scores: list[float],
    statement_html: str | None,
    tests_path: str,
) -> list[float]:
    if not test_scores or not statement_html:
        return test_scores

    sample_nums = detect_sample_test_numbers(statement_html, tests_path)
    if not sample_nums:
        return test_scores

    adjusted = list(test_scores)
    for num in sample_nums:
        if 0 < num <= len(adjusted):
            adjusted[num - 1] = 0.0
    return adjusted
