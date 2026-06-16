#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1

    # --- БЛОК 1: Примеры из условия (1-2) ---
    write_test(test_id, 1, [863961129]); test_id += 1
    write_test(test_id, 5, [1866455200, 1866455199, 1866455198, 1866455197, 1866455196]); test_id += 1

    # --- БЛОК 2: Маленькие краевые кейсы (3-10) ---
    write_test(test_id, 1, [-2000000000]); test_id += 1 # Минимальное возможное число
    write_test(test_id, 1, [2000000000]); test_id += 1  # Максимальное возможное число
    write_test(test_id, 2, [5, 4]); test_id += 1         # Массив из 2 элементов в обратном порядке
    write_test(test_id, 2, [4, 5]); test_id += 1         # Массив из 2 элементов в правильном порядке
    write_test(test_id, 10, [123456789] * 10); test_id += 1 # Все элементы абсолютно одинаковы
    write_test(test_id, 15, list(range(500, 515))); test_id += 1 # Уже отсортирован
    write_test(test_id, 15, list(range(515, 500, -1))); test_id += 1 # Отсортирован наоборот
    write_test(test_id, 20, [0] * 20); test_id += 1      # Массив из одних нулей

    # --- БЛОК 3: Большие числа и переходы через границы (11-20) ---
    # Около нижней границы int32
    min_base = -1999900000
    write_test(test_id, 50, [min_base + random.randint(0, 1000) for _ in range(50)]); test_id += 1
    # Около верхней границы int32
    max_base = 1999900000
    write_test(test_id, 50, [max_base - random.randint(0, 1000) for _ in range(50)]); test_id += 1
    # Переход через 0 (отрицательные и положительные вместе)
    write_test(test_id, 100, [random.randint(-5000, 5000) for _ in range(100)]); test_id += 1
    
    # Генерируем еще 7 тестов с экстремальными базовыми значениями, но маленьким N
    for _ in range(7):
        base = random.choice([-19 * 10**8, -10**9, -5 * 10**8, 5 * 10**8, 10**9, 19 * 10**8])
        arr = [base + random.randint(0, 50000) for _ in range(30)]
        write_test(test_id, len(arr), arr)
        test_id += 1

    # --- БЛОК 4: Тесты на структуру разброса (21-30) ---
    # Разброс ровно 10^7 (максимальный по условию)
    max_diff = 10**7
    for _ in range(5):
        base = random.randint(-15 * 10**8, 15 * 10**8)
        arr = [base, base + max_diff] + [base + random.randint(0, max_diff) for _ in range(48)]
        random.shuffle(arr)
        write_test(test_id, len(arr), arr)
        test_id += 1
    # Разброс минимальный (все числа либо X, либо X+1)
    for _ in range(5):
        base = random.randint(-15 * 10**8, 15 * 10**8)
        arr = [base + random.choice([0, 1]) for _ in range(50)]
        write_test(test_id, len(arr), arr)
        test_id += 1

    # --- БЛОК 5: Средние случайные тесты (31-45) ---
    for _ in range(15):
        n = random.randint(1000, 10000)
        base = random.randint(-18 * 10**8, 18 * 10**8)
        curr_diff = random.randint(10**5, 10**7)
        arr = [base + random.randint(0, curr_diff) for _ in range(n)]
        write_test(test_id, n, arr)
        test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (46-60) ---
    # N = 100 000. Проверяют скорость ввода-вывода и лимиты памяти
    max_n = 100000
    
    # 46-50: Максимальный N, случайные числа с максимальным разбросом
    for _ in range(5):
        base = random.randint(-15 * 10**8, 15 * 10**8)
        arr = [base, base + max_diff] + [base + random.randint(0, max_diff) for _ in range(max_n - 2)]
        random.shuffle(arr)
        write_test(test_id, max_n, arr)
        test_id += 1

    # 51-55: Максимальный N, массив уже отсортирован (худший случай для некоторых реализаций QuickSort)
    for _ in range(5):
        base = random.randint(-15 * 10**8, 15 * 10**8)
        arr = sorted([base + random.randint(0, max_diff) for _ in range(max_n)])
        write_test(test_id, max_n, arr)
        test_id += 1

    # 56-60: Максимальный N, массив отсортирован в обратном порядке
    for _ in range(5):
        base = random.randint(-15 * 10**8, 15 * 10**8)
        arr = sorted([base + random.randint(0, max_diff) for _ in range(max_n)], reverse=True)
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
    # Фиксируем seed для полной воспроизводимости пакета тестов
    random.seed(42)
    generate_tests()