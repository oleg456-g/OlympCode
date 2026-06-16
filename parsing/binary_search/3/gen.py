#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    INT_MAX = 10**9
    INT_MIN = -10**9

    # --- БЛОК 1: Пример из условия (1) ---
    write_test(test_id, 3, [1, 2, 1], 4, [0, 1, 2, 3]); test_id += 1

    # --- БЛОК 2: Маленькие краевые кейсы (2-12) ---
    write_test(test_id, 1, [5], 1, [5]); test_id += 1               # Ровно 1 совпадение
    write_test(test_id, 1, [5], 1, [3]); test_id += 1               # 0 совпадений при N=1, M=1
    write_test(test_id, 5, [1, 1, 1, 1, 1], 1, [1]); test_id += 1   # Все элементы первого массива одинаковы (ответ: 5)
    write_test(test_id, 2, [10, 20], 3, [10, 15, 20]); test_id += 1 # Граничные и отсутствующие элементы
    write_test(test_id, 4, [-5, -5, 0, 5], 4, [-5, 0, 5, 10]); test_id += 1 # Отрицательные числа
    write_test(test_id, 1, [0], 5, [0, 0, 0, 0, 0]); test_id += 1   # Повторяющиеся запросы во втором массиве
    write_test(test_id, 3, [1, 2, 3], 1, [4]); test_id += 1         # Элемента гарантированно нет
    write_test(test_id, 5, [1, 2, 3, 4, 5], 5, [5, 4, 3, 2, 1]); test_id += 1 # Все элементы по одному разу в обратном порядке
    write_test(test_id, 2, [0, 0], 2, [0, 0]); test_id += 1         # Только нули в обоих массивах
    write_test(test_id, 4, [1, 2, 3, 4], 2, [5, 6]); test_id += 1   # Полное непересечение множеств

    # --- БЛОК 3: Граничные значения диапазона (11-20) ---
    write_test(test_id, 3, [INT_MIN, 0, INT_MAX], 3, [INT_MIN, 0, INT_MAX]); test_id += 1
    write_test(test_id, 2, [INT_MIN, INT_MAX], 2, [INT_MIN + 1, INT_MAX - 1]); test_id += 1
    
    # 8 небольших случайных тестов с большими числами близко к лимитам
    for _ in range(8):
        n = random.randint(5, 20)
        m = random.randint(5, 20)
        arr1 = [random.randint(INT_MIN, INT_MAX) for _ in range(n)]
        arr2 = [random.choice(arr1) if random.random() > 0.4 else random.randint(INT_MIN, INT_MAX) for _ in range(m)]
        write_test(test_id, n, arr1, m, arr2); test_id += 1

    # --- БЛОК 4: Тесты со специфической частотностью и структурами (21-35) ---
    # Первый массив полностью заполнен дубликатами нескольких ключей
    for _ in range(5):
        n = random.randint(100, 500)
        m = random.randint(50, 100)
        pool = [random.randint(-10, 10) for _ in range(5)]
        arr1 = [random.choice(pool) for _ in range(n)]
        arr2 = [random.randint(-15, 15) for _ in range(m)]
        write_test(test_id, n, arr1, m, arr2); test_id += 1

    # Второй массив полностью состоит из элементов, которых нет в первом
    for _ in range(5):
        n = random.randint(100, 300)
        m = random.randint(100, 300)
        arr1 = [random.randint(0, 500) * 2 for _ in range(n)]       # Только четные числа
        arr2 = [random.randint(0, 500) * 2 + 1 for _ in range(m)]   # Только нечетные числа
        write_test(test_id, n, arr1, m, arr2); test_id += 1

    # Первый массив упорядочен, второй неупорядочен (проверка бинпоиска по границам серий)
    for _ in range(5):
        n = random.randint(500, 1000)
        m = random.randint(200, 500)
        arr1 = sorted([random.randint(-1000, 1000) for _ in range(n)])
        arr2 = [random.randint(-1000, 1000) for _ in range(m)]
        write_test(test_id, n, arr1, m, arr2); test_id += 1

    # --- БЛОК 5: Средние случайные тесты (36-45) ---
    for _ in range(10):
        n = random.randint(5000, 15000)
        m = random.randint(5000, 15000)
        arr1 = [random.randint(-10**6, 10**6) for _ in range(n)]
        arr2 = [random.choice(arr1) if random.random() > 0.5 else random.randint(-10**6, 10**6) for _ in range(m)]
        write_test(test_id, n, arr1, m, arr2); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (46-60) ---
    # Ровно N = 100 000, M = 100 000. Любое наивное решение за O(N*M) упадет по TLE.
    max_nm = 100000
    
    # 46-50: Полностью случайные большие массивы во всем диапазоне знакового int
    for _ in range(5):
        arr1 = [random.randint(INT_MIN, INT_MAX) for _ in range(max_nm)]
        arr2 = [random.randint(INT_MIN, INT_MAX) for _ in range(max_nm)]
        write_test(test_id, max_nm, arr1, max_nm, arr2); test_id += 1

    # 51-55: Все элементы второго массива гарантированно присутствуют в первом и имеют много дубликатов
    for _ in range(5):
        small_pool = [random.randint(INT_MIN, INT_MAX) for _ in range(100)] # Всего 100 уникальных чисел
        arr1 = [random.choice(small_pool) for _ in range(max_nm)]
        arr2 = [random.choice(small_pool) for _ in range(max_nm)]
        write_test(test_id, max_nm, arr1, max_nm, arr2); test_id += 1

    # 56-60: Худший случай для хэш-таблиц / бинпоиска (все элементы первого массива уникальны и отсортированы)
    for _ in range(5):
        arr1 = sorted(list(set([random.randint(INT_MIN, INT_MAX) for _ in range(max_nm * 2)])))[:max_nm]
        arr2 = [random.choice(arr1) if i % 2 == 0 else random.randint(INT_MIN, INT_MAX) for i in range(max_nm)]
        write_test(test_id, max_nm, arr1, max_nm, arr2); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, array1, m, array2):
    """Вспомогательная функция для записи теста в файл формата XX.in"""
    filename = f"tests/{str(test_id).zfill(2)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n}\n")
        f.write(" ".join(map(str, array1)) + "\n")
        f.write(f"{m}\n")
        f.write(" ".join(map(str, array2)) + "\n")

if __name__ == "__main__":
    # Фиксируем seed для стабильной генерации пакета тестов
    random.seed(303030)
    generate_tests()