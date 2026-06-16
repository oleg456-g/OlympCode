#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    INT_MAX = 2 * 10**9
    INT_MIN = -2 * 10**9

    # --- БЛОК 1: Пример из условия (1) ---
    write_test(test_id, 5, 5, [1, 3, 5, 7, 9], [2, 4, 8, 1, 6]); test_id += 1

    # --- БЛОК 2: Маленькие краевые кейсы (2-12) ---
    write_test(test_id, 1, 1, [10], [10]); test_id += 1             # Точное совпадение при N=1
    write_test(test_id, 1, 2, [10], [5, 15]); test_id += 1           # Слева и справа от единственного элемента
    write_test(test_id, 2, 1, [10, 20], [15]); test_id += 1          # Ровно посередине двух элементов (вывод меньшего: 10)
    write_test(test_id, 2, 1, [10, 20], [14]); test_id += 1          # Ближе к левому
    write_test(test_id, 2, 1, [10, 20], [16]); test_id += 1          # Ближе к правому
    write_test(test_id, 2, 2, [10, 20], [2, 25]); test_id += 1       # За пределами всего массива (меньше первого, больше последнего)
    write_test(test_id, 4, 4, [1, 1, 5, 5], [1, 5, 0, 6]); test_id += 1 # Массив с дубликатами на границах
    write_test(test_id, 5, 3, [-10, -5, 0, 5, 10], [-8, -2, 3]); test_id += 1 # Отрицательные и положительные числа
    write_test(test_id, 10, 2, [0]*10, [-5, 5]); test_id += 1        # Все элементы массива одинаковые нули
    write_test(test_id, 3, 2, [10, 12, 14], [11, 13]); test_id += 1  # Точки строго посередине между парами

    # --- БЛОК 3: Граничные значения диапазона (13-22) ---
    write_test(test_id, 3, 3, [INT_MIN, 0, INT_MAX], [INT_MIN, 0, INT_MAX]); test_id += 1
    write_test(test_id, 2, 4, [INT_MIN + 10, INT_MAX - 10], [INT_MIN, INT_MIN + 5, INT_MAX, INT_MAX - 5]); test_id += 1
    
    # 8 небольших случайных тестов в экстремальных диапазонах знакового 32-битного int
    for _ in range(8):
        n = random.randint(5, 20)
        k = random.randint(5, 20)
        arr = sorted([random.randint(INT_MIN, INT_MAX) for _ in range(n)])
        queries = [random.randint(INT_MIN, INT_MAX) for _ in range(k)]
        write_test(test_id, n, k, arr, queries); test_id += 1

    # --- БЛОК 4: Разряженные структуры и проверка на полусумму (23-35) ---
    # Тесты с шагом 2 и нечетными запросами (проверка правила "выведите меньшее")
    for _ in range(5):
        n = random.randint(50, 150)
        k = random.randint(20, 50)
        start = random.randint(-10000, 10000)
        arr = [start + i * 2 for i in range(n)]
        # Запрос вида `element + 1` находится ровно посередине между `element` и `element + 2`
        queries = [random.choice(arr) + 1 for _ in range(k)]
        write_test(test_id, n, k, arr, queries); test_id += 1

    # Плотные кластеры одинаковых чисел
    for _ in range(8):
        n = random.randint(100, 500)
        k = random.randint(50, 100)
        arr = sorted([random.choice([-500, 0, 500]) for _ in range(n)])
        queries = [random.randint(-1000, 1000) for _ in range(k)]
        write_test(test_id, n, k, arr, queries); test_id += 1

    # --- БЛОК 5: Средние случайные тесты (36-45) ---
    for _ in range(10):
        n = random.randint(2000, 8000)
        k = random.randint(1000, 4000)
        arr = sorted([random.randint(-10**8, 10**8) for _ in range(n)])
        queries = [random.randint(-10**8, 10**8) for _ in range(k)]
        write_test(test_id, n, k, arr, queries); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (46-60) ---
    # Ровно n = 100 000, k = 100 000. Проверяет асимптотику O(K log N) и оптимизацию ввода-вывода.
    max_nk = 100000
    
    # 46-50: Полностью случайные числа во всем диапазоне от -2*10^9 до 2*10^9
    for _ in range(5):
        arr = sorted([random.randint(INT_MIN, INT_MAX) for _ in range(max_nk)])
        queries = [random.randint(INT_MIN, INT_MAX) for _ in range(max_nk)]
        write_test(test_id, max_nk, max_nk, arr, queries); test_id += 1

    # 51-55: Запросы гарантированно меньше минимального элемента массива (проверка левой границы)
    for _ in range(5):
        arr = sorted([random.randint(0, INT_MAX) for _ in range(max_nk)])
        queries = [random.randint(INT_MIN, -1) for _ in range(max_nk)]
        write_test(test_id, max_nk, max_nk, arr, queries); test_id += 1

    # 56-60: Запросы гарантированно больше максимального элемента массива (проверка правой границы)
    for _ in range(5):
        arr = sorted([random.randint(INT_MIN, 0) for _ in range(max_nk)])
        queries = [random.randint(1, INT_MAX) for _ in range(max_nk)]
        write_test(test_id, max_nk, max_nk, arr, queries); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, k, array, queries):
    """Вспомогательная функция для записи теста в файл формата XX.in"""
    filename = f"tests/{str(test_id).zfill(2)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n} {k}\n")
        f.write(" ".join(map(str, array)) + "\n")
        f.write(" ".join(map(str, queries)) + "\n")

if __name__ == "__main__":
    # Фиксируем seed для стабильной генерации пакета тестов
    random.seed(202602)
    generate_tests()