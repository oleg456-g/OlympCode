#!/usr/bin/env python3
import random
import os

def generate_tests():
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1

    # --- БЛОК 1: Примеры из условия (1-2) ---
    write_test(test_id, ["2", "20", "004", "66"]); test_id += 1
    write_test(test_id, ["3"]); test_id += 1

    # --- БЛОК 2: Краевые и минимальные тесты (3-20) ---
    write_test(test_id, ["0", "1"]); test_id += 1
    write_test(test_id, ["10", "1"]); test_id += 1 # Важно: 1 + 10 > 10 + 1 (110 > 101)
    write_test(test_id, ["9", "99", "999"]); test_id += 1
    write_test(test_id, ["000", "00", "5"]); test_id += 1
    write_test(test_id, ["10", "100", "1000", "2"]); test_id += 1
    write_test(test_id, ["4", "42", "421", "423"]); test_id += 1
    
    # Стресс-тест компаратора на эквивалентных строках (чтобы поймать некорректный Strict Weak Ordering)
    write_test(test_id, ["12"] * 50 + ["3"]); test_id += 1
    write_test(test_id, ["45", "4545", "454545", "9"]); test_id += 1
    write_test(test_id, ["0"] * 99 + ["1"]); test_id += 1
    
    # Тесты с максимальной длиной одной строки (100 цифр)
    write_test(test_id, ["9" * 100, "9" * 99 + "8"]); test_id += 1
    write_test(test_id, ["1" + "0" * 99, "2"]); test_id += 1
    write_test(test_id, ["8" * 100, "88"]); test_id += 1

    # Добиваем блок вариациями префиксов
    for pref in ["2", "31", "7", "90"]:
        write_test(test_id, [pref, pref * 2, pref * 3, "9"]); test_id += 1
    write_test(test_id, ["123", "12312", "1231234"]); test_id += 1

    # --- БЛОК 3: Тесты на ловушки префиксов и длин (21-50) ---
    for _ in range(30):
        # Генерируем строки, которые часто вызывают ошибки при посимвольном сравнении без склеивания
        base = "".join(random.choices("0123456789", k=random.randint(1, 4)))
        if not base or base.startswith("0"): 
            base = "5"
        lines = [base, base + base[:random.randint(1, len(base))], base * 2, "9"]
        # Добавим случайных цифр
        for _ in range(random.randint(5, 15)):
            lines.append("".join(random.choices("0123456789", k=random.randint(1, 10))))
        # Гарантируем ненулевой первый символ хоть где-то
        lines.append("7")
        write_test(test_id, lines); test_id += 1

    # --- БЛОК 4: Большое количество одинаковых элементов (51-80) ---
    # Падение неверных компараторов (Strict Weak Ordering UB)
    for _ in range(30):
        n_lines = random.randint(40, 100)
        # Берем 1-3 вида повторяющихся строк
        pool = ["".join(random.choices("0123456789", k=random.randint(1, 5))) for _ in range(3)]
        pool.append("8") # Гарантия ненулевого элемента
        lines = [random.choice(pool) for _ in range(n_lines)]
        write_test(test_id, lines); test_id += 1

    # --- БЛОК 5: Средние случайные тесты (81-100) ---
    for _ in range(20):
        n_lines = random.randint(30, 70)
        lines = []
        for _ in range(n_lines):
            lines.append("".join(random.choices("0123456789", k=random.randint(1, 30))))
        lines.append("1")
        write_test(test_id, lines); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (101-120) ---
    # 100 строк по 100 цифр в каждой
    for _ in range(20):
        n_lines = 100
        lines = []
        for _ in range(n_lines):
            lines.append("".join(random.choices("0123456789", k=100)))
        # Гарантируем валидность по условию
        lines[0] = "9" + lines[0][1:]
        write_test(test_id, lines); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, lines):
    """Вспомогательная функция для записи теста"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        # По условию строки подаются на вход, разделенные переносом строки
        f.write("\n".join(lines) + "\n")

if __name__ == "__main__":
    random.seed(987654)
    generate_tests()