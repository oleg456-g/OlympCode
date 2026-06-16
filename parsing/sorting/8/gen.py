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
    write_test(test_id, 2, [3, 1]); test_id += 1

    # --- БЛОК 2: Маленькие краевые кейсы (2-10) ---
    write_test(test_id, 1, [0]); test_id += 1              # Один нуль
    write_test(test_id, 1, [INT_MAX]); test_id += 1       # Минимальный массив, максимальное число
    write_test(test_id, 1, [INT_MIN]); test_id += 1       # Минимальный массив, минимальное число
    write_test(test_id, 2, [10, 10]); test_id += 1         # Два одинаковых элемента
    write_test(test_id, 2, [-5, 5]); test_id += 1          # Два элемента в правильном порядке
    write_test(test_id, 3, [1, 2, 3]); test_id += 1        # Маленький отсортированный массив
    write_test(test_id, 3, [3, 2, 1]); test_id += 1        # Маленький обратно отсортированный массив
    write_test(test_id, 4, [2, 1, 4, 3]); test_id += 1     # Чередующийся массив
    write_test(test_id, 10, [42] * 10); test_id += 1       # Все элементы одинаковые

    # --- БЛОК 3: Граничные значения и знаки (11-20) ---
    # Только максимальные и минимальные значения (тест на аккуратность работы с типами данных)
    write_test(test_id, 20, [random.choice([INT_MIN, INT_MAX]) for _ in range(20)]); test_id += 1
    # Массив из чередующихся INT_MIN и INT_MAX
    write_test(test_id, 20, [INT_MIN if i % 2 == 0 else INT_MAX for i in range(20)]); test_id += 1
    
    # Еще 8 случайных маленьких тестов с большими числами
    for _ in range(8):
        n = random.randint(5, 50)
        arr = [random.randint(INT_MIN, INT_MAX) for _ in range(n)]
        write_test(test_id, n, arr)
        test_id += 1

    # --- БЛОК 4: Специфические структуры массивов (21-35) ---
    # 21-25: Уже отсортированные массивы разного размера (проверка лучшего случая)
    for _ in range(5):
        n = random.randint(100, 1000)
        arr = sorted([random.randint(-10**6, 10**6) for _ in range(n)])
        write_test(test_id, n, arr)
        test_id += 1
        
    # 26-30: Обратно отсортированные массивы (худший случай для некоторых других сортировок)
    for _ in range(5):
        n = random.randint(100, 1000)
        arr = sorted([random.randint(-10**6, 10**6) for _ in range(n)], reverse=True)
        write_test(test_id, n, arr)
        test_id += 1
        
    # 31-35: Массивы со множеством дубликатов (проверка устойчивости и корректности равенств)
    for _ in range(5):
        n = random.randint(200, 1000)
        pool = [random.randint(-5, 5) for _ in range(5)] # Всего 5 уникальных значений
        arr = [random.choice(pool) for _ in range(n)]
        write_test(test_id, n, arr)
        test_id += 1

    # --- БЛОК 5: Средние случайные тесты (36-45) ---
    for _ in range(10):
        n = random.randint(5000, 20000)
        arr = [random.randint(INT_MIN, INT_MAX) for _ in range(n)]
        write_test(test_id, n, arr)
        test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (46-60) ---
    # N = 100 000. Проверка асимптотики O(N log N). Любой O(N^2) здесь гарантированно упадет по TLE.
    max_n = 100000
    
    # 46-50: Полностью случайные большие массивы
    for _ in range(5):
        arr = [random.randint(INT_MIN, INT_MAX) for _ in range(max_n)]
        write_test(test_id, max_n, arr)
        test_id += 1
        
    # 51-55: Максимальный N, массив уже отсортирован
    for _ in range(5):
        arr = sorted([random.randint(INT_MIN, INT_MAX) for _ in range(max_n)])
        write_test(test_id, max_n, arr)
        test_id += 1

    # 56-60: Максимальный N, массив отсортирован в обратном порядке
    for _ in range(5):
        arr = sorted([random.randint(INT_MIN, INT_MAX) for _ in range(max_n)], reverse=True)
        write_test(test_id, max_n, arr)
        test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, array):
    """Вспомогательная функция для записи теста в файл формата XX.in"""
    filename = f"tests/{str(test_id).zfill(2)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n}\n")
        f.write(" ".join(map(str, array)) + "\n")

if __name__ == "__main__":
    # Фиксируем seed, чтобы пакет тестов всегда генерировался идентичным
    random.seed(987654321)
    generate_tests()