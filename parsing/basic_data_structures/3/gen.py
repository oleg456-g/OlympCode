#!/usr/bin/env python3
import random
import os

def generate_tests():
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_H = 10**9
    MAX_N = 10**6

    # --- БЛОК 1: Примеры из условия (1-3) ---
    write_test(test_id, 7, [2, 1, 4, 5, 1, 3, 3]); test_id += 1
    write_test(test_id, 3, [2, 1, 2]); test_id += 1
    write_test(test_id, 1, [0]); test_id += 1

    # --- БЛОК 2: Минимальные и краевые кейсы (4-15) ---
    write_test(test_id, 1, [MAX_H]); test_id += 1                     # Один элемент максимальной высоты
    write_test(test_id, 5, [0, 0, 0, 0, 0]); test_id += 1             # Все нули
    write_test(test_id, 2, [10, 10]); test_id += 1                    # Одинаковые элементы
    write_test(test_id, 3, [10, 0, 10]); test_id += 1                 # Разделенные нулем
    write_test(test_id, 4, [1, 2, 3, 4]); test_id += 1                 # Строго возрастающие
    write_test(test_id, 4, [4, 3, 2, 1]); test_id += 1                 # Строго убывающие
    write_test(test_id, 5, [1, 2, 3, 2, 1]); test_id += 1             # Пирамида
    write_test(test_id, 5, [3, 2, 1, 2, 3]); test_id += 1             # Впадина
    
    # Тесты, провоцирующие лонг-лонг переполнение (Ответ > 2^31 - 1)
    write_test(test_id, 3, [MAX_H, MAX_H, MAX_H]); test_id += 1       # 3 * 10^9 лезет только в long long
    write_test(test_id, 5, [MAX_H - i for i in range(5)]); test_id += 1
    write_test(test_id, 2, [0, MAX_H]); test_id += 1

    # --- БЛОК 3: Монотонные и ступенчатые последовательности (16-45) ---
    # Проверка глубокого захода в циклы while внутри стека
    for size in range(15, 45):
        if size % 2 == 0:
            # Ступеньки вверх
            heights = [i * 1000 for i in range(1, size + 1)]
        else:
            # Ступеньки вниз
            heights = [(size - i) * 1000 for i in range(size)]
        write_test(test_id, size, heights); test_id += 1

    # --- БЛОК 4: Пилообразные и периодические структуры (46-75) ---
    for _ in range(15):
        size = random.randint(50, 100)
        # Повторяющийся паттерн [10, 100, 10, 100 ...]
        heights = [10 if i % 2 == 0 else 100 for i in range(size)]
        write_test(test_id, size, heights); test_id += 1

    for _ in range(15):
        size = random.randint(50, 100)
        # Случайные плато (длинные группы одинаковых высот)
        heights = []
        for _ in range(size // 5):
            h = random.randint(1, 1000)
            heights.extend([h] * 5)
        write_test(test_id, len(heights), heights); test_id += 1

    # --- БЛОК 5: Средние случайные тесты (76-100) ---
    for _ in range(25):
        size = random.randint(1000, 5000)
        heights = [random.randint(0, MAX_H) for _ in range(size)]
        write_test(test_id, size, heights); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (101-120) ---
    # Огромные тесты до N = 1 000 000 для проверки лимитов времени (1.0 с) и памяти.
    
    # 101-104: Максимальный размер, все высоты максимальны (Жесткий long long overflow тест, ответ 10^15)
    for _ in range(4):
        write_test(test_id, MAX_N, [MAX_H] * MAX_N); test_id += 1

    # 105-108: Все элементы одинаковые, но маленькие
    for _ in range(4):
        write_test(test_id, MAX_N, [1] * MAX_N); test_id += 1

    # 109-112: Строго возрастающий массив длины 1 000 000 (Проверка худшего случая для стека в одну сторону)
    for _ in range(4):
        # Шаг 1000, чтобы не выйти за MAX_H
        heights = [i * 1000 for i in range(MAX_N)]
        write_test(test_id, MAX_N, heights); test_id += 1

    # 113-116: Строго убывающий массив длины 1 000 000 (Проверка худшего случая для стека в обратную сторону)
    for _ in range(4):
        heights = [(MAX_N - i) * 1000 for i in range(MAX_N)]
        write_test(test_id, MAX_N, heights); test_id += 1

    # 117-120: Полностью случайные массивы максимального размера
    for _ in range(4):
        heights = [random.randint(0, MAX_H) for _ in range(MAX_N)]
        write_test(test_id, MAX_N, heights); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, heights):
    """Вспомогательная функция для записи файла теста"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n}\n")
        f.write(" ".join(map(str, heights)) + "\n")

if __name__ == "__main__":
    random.seed(456789)
    generate_tests()