#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_VAL = 10000
    MIN_VAL = -10000

    # --- БЛОК 1: Примеры из условия (1-2) ---
    write_test(test_id, 5, [1, 3, 4, 2, 5]); test_id += 1
    write_test(test_id, 4, [10, -5, -5, 0]); test_id += 1

    # --- БЛОК 2: Маленькие краевые кейсы (3-10) ---
    write_test(test_id, 1, [0]); test_id += 1               # Один ноль
    write_test(test_id, 1, [MAX_VAL]); test_id += 1         # Только максимальное значение
    write_test(test_id, 1, [MIN_VAL]); test_id += 1         # Только минимальное значение (тест на offset)
    write_test(test_id, 2, [0, 0]); test_id += 1             # Два одинаковых элемента
    write_test(test_id, 2, [5, -5]); test_id += 1           # Два элемента, один отрицательный
    write_test(test_id, 10, [7] * 10); test_id += 1         # Все элементы одинаковые
    write_test(test_id, 15, list(range(-7, 8))); test_id += 1 # Уже отсортирован (переход через ноль)
    write_test(test_id, 15, list(range(7, -8, -1))); test_id += 1 # Отсортирован в обратном порядке

    # --- БЛОК 3: Граничные значения диапазона (11-20) ---
    # Массивы, состоящие только из экстремальных значений -10000 и 10000
    write_test(test_id, 30, [random.choice([MIN_VAL, MAX_VAL]) for _ in range(30)]); test_id += 1
    write_test(test_id, 20, [MIN_VAL if i % 2 == 0 else MAX_VAL for i in range(20)]); test_id += 1
    
    # Случайные маленькие тесты в узких диапазонах вокруг границ
    for _ in range(4):
        n = random.randint(5, 40)
        arr = [random.randint(MIN_VAL, MIN_VAL + 100) for _ in range(n)] # Только глубокий минус
        write_test(test_id, n, arr); test_id += 1
    for _ in range(4):
        n = random.randint(5, 40)
        arr = [random.randint(MAX_VAL - 100, MAX_VAL) for _ in range(n)] # Только высокий плюс
        write_test(test_id, n, arr); test_id += 1

    # --- БЛОК 4: Разряженные и плотные распределения (21-35) ---
    # Массивы со множеством дубликатов (мало уникальных ключей)
    for _ in range(5):
        n = random.randint(100, 500)
        keys = [random.randint(-10, 10) for _ in range(3)] # Всего 3 уникальных числа
        arr = [random.choice(keys) for _ in range(n)]
        write_test(test_id, n, arr); test_id += 1
        
    # Массивы, где почти все элементы уникальны (широкий разброс)
    for _ in range(10):
        n = random.randint(100, 1000)
        arr = [random.randint(MIN_VAL, MAX_VAL) for _ in range(n)]
        write_test(test_id, n, arr); test_id += 1

    # --- БЛОК 5: Средние случайные тесты (36-45) ---
    for _ in range(10):
        n = random.randint(5000, 20000)
        arr = [random.randint(MIN_VAL, MAX_VAL) for _ in range(n)]
        write_test(test_id, n, arr); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (46-60) ---
    # N = 100 000. Проверка линейного времени O(N + K). Любой O(N^2) здесь гарантированно выдаст TLE.
    max_n = 100000
    
    # 46-50: Полностью случайные массивы максимального размера
    for _ in range(5):
        arr = [random.randint(MIN_VAL, MAX_VAL) for _ in range(max_n)]
        write_test(test_id, max_n, arr); test_id += 1
        
    # 51-55: Максимальный N, все элементы — нули (проверка вырожденного случая)
    for _ in range(5):
        arr = [0] * max_n
        write_test(test_id, max_n, arr); test_id += 1

    # 56-60: Максимальный N, содержащий абсолютно все возможные числа из диапазона [-10000, 10000]
    # Гарантирует, что каждый индекс массива подсчета будет задействован хотя бы раз
    for _ in range(5):
        # Заполняем сначала весь диапазон гарантированно
        arr = list(range(MIN_VAL, MAX_VAL + 1))
        # Добираем до 100 000 случайными числами
        arr.extend([random.randint(MIN_VAL, MAX_VAL) for _ in range(max_n - len(arr))])
        random.shuffle(arr)
        write_test(test_id, max_n, arr); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, array):
    """Вспомогательная функция для записи теста в файл формата XX.in"""
    filename = f"tests/{str(test_id).zfill(2)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n}\n")
        f.write(" ".join(map(str, array)) + "\n")

if __name__ == "__main__":
    # Фиксируем seed для стабильности генерации пакета тестов
    random.seed(55555)
    generate_tests()