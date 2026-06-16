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
    # В примере из условия была опечатка (в выводе 4 строки вместо 5). Исправляем до корректного.
    write_test(test_id, 10, 5, list(range(1, 11)), [-2, 0, 4, 9, 12]); test_id += 1

    # --- БЛОК 2: Маленькие краевые кейсы (2-12) ---
    write_test(test_id, 1, 1, [5], [5]); test_id += 1              # Один элемент, элемент есть (YES)
    write_test(test_id, 1, 1, [5], [3]); test_id += 1              # Один элемент, элемента нет (NO)
    write_test(test_id, 2, 2, [1, 10], [1, 10]); test_id += 1       # Границы двух элементов (YES, YES)
    write_test(test_id, 2, 2, [1, 10], [0, 11]); test_id += 1       # Мимо границ двух элементов (NO, NO)
    write_test(test_id, 2, 1, [1, 10], [5]); test_id += 1           # Между двумя элементами (NO)
    write_test(test_id, 5, 5, [2, 4, 6, 8, 10], [2, 4, 6, 8, 10]); test_id += 1 # Все элементы присутствуют
    write_test(test_id, 5, 5, [2, 4, 6, 8, 10], [1, 3, 5, 7, 9]); test_id += 1  # Все элементы мимо (между)
    write_test(test_id, 10, 2, [5]*10, [5, 4]); test_id += 1        # Массив из одинаковых элементов
    write_test(test_id, 5, 3, [-10, -5, 0, 5, 10], [-5, 0, 5]); test_id += 1   # Отрицательные и положительные

    # --- БЛОК 3: Граничные значения диапазона (11-20) ---
    write_test(test_id, 3, 3, [INT_MIN, 0, INT_MAX], [INT_MIN, 0, INT_MAX]); test_id += 1 # Экстремальные числа
    write_test(test_id, 2, 2, [INT_MIN, INT_MAX], [INT_MIN - 1, INT_MAX + 1]); test_id += 1 # За пределами инта по условию
    
    # 8 маленьких случайных тестов с большими числами
    for _ in range(8):
        n = random.randint(5, 20)
        k = random.randint(5, 20)
        arr = sorted([random.randint(INT_MIN, INT_MAX) for _ in range(n)])
        queries = [random.choice(arr) if random.random() > 0.5 else random.randint(INT_MIN, INT_MAX) for _ in range(k)]
        write_test(test_id, n, k, arr, queries); test_id += 1

    # --- БЛОК 4: Разряженные и специальные структуры (21-35) ---
    # Арифметические прогрессии с шагом
    for _ in range(5):
        n = random.randint(100, 500)
        k = random.randint(50, 100)
        step = random.randint(2, 10)
        start = random.randint(-10000, 10000)
        arr = [start + i * step for i in range(n)]
        # Запросы: часть гарантированно внутри, часть гарантированно мимо из-за шага
        queries = []
        for _ in range(k):
            if random.random() > 0.5:
                queries.append(random.choice(arr))
            else:
                queries.append(random.choice(arr) + 1) # Сдвиг на 1 гарантирует промах, если step > 1
        write_test(test_id, n, k, arr, queries); test_id += 1

    # Массивы с плотными дубликатами
    for _ in range(10):
        n = random.randint(200, 1000)
        k = random.randint(100, 200)
        arr = sorted([random.choice([-100, -50, 0, 50, 100]) for _ in range(n)])
        queries = [random.randint(-120, 120) for _ in range(k)]
        write_test(test_id, n, k, arr, queries); test_id += 1

    # --- БЛОК 5: Средние случайные тесты (36-45) ---
    for _ in range(10):
        n = random.randint(5000, 15000)
        k = random.randint(2000, 5000)
        arr = sorted([random.randint(-10**7, 10**7) for _ in range(n)])
        queries = [random.choice(arr) if random.random() > 0.4 else random.randint(-10**7, 10**7) for _ in range(k)]
        write_test(test_id, n, k, arr, queries); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (46-60) ---
    # Ровно n = 100 000, k = 100 000. Проверяет асимптотику поиска и скорость I/O флагов.
    max_nk = 100000
    
    # 46-50: Полностью случайные большие массивы (смесь YES и NO)
    for _ in range(5):
        arr = sorted([random.randint(INT_MIN, INT_MAX) for _ in range(max_nk)])
        # Делаем так, чтобы ~50% запросов гарантированно находились
        queries = [random.choice(arr) if i % 2 == 0 else random.randint(INT_MIN, INT_MAX) for i in range(max_nk)]
        write_test(test_id, max_nk, max_nk, arr, queries); test_id += 1

    # 51-55: Все ответы должны быть YES (все запросы выдернуты из массива)
    for _ in range(5):
        arr = sorted([random.randint(INT_MIN, INT_MAX) for _ in range(max_nk)])
        queries = [random.choice(arr) for _ in range(max_nk)]
        random.shuffle(queries)
        write_test(test_id, max_nk, max_nk, arr, queries); test_id += 1

    # 56-60: Все ответы должны быть NO (запросы гарантированно не пересекаются с массивом)
    # Генерируем массив из четных чисел, а запросы из нечетных
    for _ in range(5):
        arr = sorted([random.randint(MIN_VAL_HALF := -5 * 10**8, MAX_VAL_HALF := 5 * 10**8) * 2 for _ in range(max_nk)])
        queries = [random.randint(MIN_VAL_HALF, MAX_VAL_HALF) * 2 + 1 for _ in range(max_nk)]
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
    # Фиксируем seed для воспроизводимости
    random.seed(101010)
    generate_tests()