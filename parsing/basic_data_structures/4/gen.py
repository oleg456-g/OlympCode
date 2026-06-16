#!/usr/bin/env python3
import random
import os

def generate_tests():
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1

    # --- БЛОК 1: Примеры из условия (1-2) ---
    write_test(test_id, 5, 6, [
        [1, 0, 0, 0, 1, 0],
        [0, 0, 0, 0, 1, 0],
        [0, 0, 1, 0, 0, 0],
        [0, 0, 0, 0, 0, 0],
        [0, 0, 1, 0, 0, 0]
    ]); test_id += 1

    write_test(test_id, 4, 4, [
        [0, 0, 0, 0],
        [0, 1, 0, 1],
        [0, 0, 0, 0],
        [1, 1, 0, 0]
    ]); test_id += 1

    # --- БЛОК 2: Минимальные и краевые кейсы (3-9) ---
    write_test(test_id, 1, 1, [[0]]); test_id += 1                  # 1х1 Белая
    write_test(test_id, 1, 1, [[1]]); test_id += 1                  # 1х1 Черная
    write_test(test_id, 1, 5, [[0, 0, 0, 0, 0]]); test_id += 1      # Строка полностью белая
    write_test(test_id, 1, 5, [[1, 1, 1, 1, 1]]); test_id += 1      # Строка полностью черная
    write_test(test_id, 5, 1, [[0], [0], [0], [0], [0]]); test_id += 1  # Столбец полностью белый
    write_test(test_id, 2, 2, [[0, 1], [1, 0]]); test_id += 1      # Шахматная 2х2
    write_test(test_id, 3, 3, [[0, 0, 0], [0, 1, 0], [0, 0, 0]]); test_id += 1 # Рамка

    # --- БЛОК 3: Различные геометрии матриц (10-39) ---
    # Вытянутые матрицы (ровно 30 тестов)
    for size in range(10, 40):
        if size % 2 == 0:
            grid = [[0] * size for _ in range(2)]
        else:
            grid = [[random.choice([0, 1]), random.choice([0, 1])] for _ in range(size)]
        write_test(test_id, len(grid), len(grid[0]), grid); test_id += 1

    # --- БЛОК 4: Специальные паттерны и структуры (40-69) ---
    # Полосы (ровно 30 тестов)
    for _ in range(15):
        n, m = random.randint(10, 30), random.randint(10, 30)
        grid = [[i % 2] * m for i in range(n)]
        write_test(test_id, n, m, grid); test_id += 1

    for _ in range(15):
        n, m = random.randint(10, 30), random.randint(10, 30)
        grid = [[j % 2 for j in range(m)] for _ in range(n)]
        write_test(test_id, n, m, grid); test_id += 1

    # --- БЛОК 5: Случайные тесты среднего размера (70-100) ---
    # Ровно 31 тест для точного баланса до 100
    for _ in range(31):
        n = random.randint(50, 150)
        m = random.randint(50, 150)
        grid = [[0 if random.random() < 0.7 else 1 for _ in range(m)] for _ in range(n)]
        write_test(test_id, n, m, grid); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (101-120) ---
    # Ровно 20 тестов до размеров 5000 х 5000
    big_sizes = [(1000, 1000), (2000, 2000), (5000, 1000), (1000, 5000)]
    
    # 101-104: Полностью белые матрицы
    for n, m in big_sizes:
        write_test_huge_white(test_id, n, m)
        test_id += 1

    # 105-108: Полностью черные матрицы
    for n, m in big_sizes:
        write_test_huge_black(test_id, n, m)
        test_id += 1

    # 109-114: Большие случайные матрицы с внедренными белыми прямоугольниками (6 тестов)
    for _ in range(6):
        n, m = 2000, 2000
        grid = [[random.choice([0, 1]) for _ in range(m)] for _ in range(n)]
        rs, re = random.randint(100, 500), random.randint(1200, 1800)
        cs, ce = random.randint(100, 500), random.randint(1200, 1800)
        for r in range(rs, re):
            for c in range(cs, ce):
                grid[r][c] = 0
        write_test(test_id, n, m, grid); test_id += 1

    # 115-120: Предельный размер 5000 х 5000 с разреженными черными точками (6 тестов)
    for _ in range(6):
        n, m = 5000, 5000
        filename = f"tests/{str(test_id).zfill(3)}.in"
        with open(filename, "w", encoding="utf-8") as f:
            f.write(f"{n} {m}\n")
            for _ in range(n):
                row = [0 if random.random() < 0.98 else 1 for _ in range(m)]
                f.write(" ".join(map(str, row)) + "\n")
        test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")


def write_test(test_id, n, m, grid):
    """Вспомогательная функция для записи обычного теста"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n} {m}\n")
        for row in grid:
            f.write(" ".join(map(str, row)) + "\n")

def write_test_huge_white(test_id, n, m):
    """Оптимальная по памяти запись огромного белого поля"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n} {m}\n")
        row_str = " ".join(["0"] * m) + "\n"
        for _ in range(n):
            f.write(row_str)

def write_test_huge_black(test_id, n, m):
    """Оптимальная по памяти запись огромного черного поля"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n} {m}\n")
        row_str = " ".join(["1"] * m) + "\n"
        for _ in range(n):
            f.write(row_str)


if __name__ == "__main__":
    random.seed(555111)
    generate_tests()