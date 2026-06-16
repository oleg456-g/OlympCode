#!/usr/bin/env python3
import random
import os
import heapq

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_N = 100000
    MAX_VAL = 10**9

    # --- БЛОК 1: Примеры из условия (2 теста: 001-002) ---
    write_test(test_id, 2, [2, 6]); test_id += 1
    write_test(test_id, 3, [6, 2, 4]); test_id += 1

    # --- БЛОК 2: Краевые кейсы и минимальные ограничения (13 тестов: 003-015) ---
    write_test(test_id, 2, [1, 1]); test_id += 1                     # Абсолютный минимум
    write_test(test_id, 2, [MAX_VAL, MAX_VAL]); test_id += 1         # Два максимума (тест на лонг лонг в сумме слияния)
    write_test(test_id, 3, [MAX_VAL, MAX_VAL, MAX_VAL]); test_id += 1 # Три максимума
    write_test(test_id, 4, [1, 1, 1, 1]); test_id += 1               # Идеальное сбалансированное дерево
    write_test(test_id, 5, [1, 2, 3, 4, 5]); test_id += 1            # Последовательные числа
    write_test(test_id, 6, [10, 10, 10, 10, 10, 10]); test_id += 1   # Все элементы равны
    
    # Геометрическая прогрессия (глубокое несбалансированное дерево)
    write_test(test_id, 5, [1, 2, 4, 8, 16]); test_id += 1
    write_test(test_id, 5, [16, 8, 4, 2, 1]); test_id += 1           # Проверка на то, что сортировка не влияет
    
    # Один гигантский элемент и много маленьких
    write_test(test_id, 6, [1, 1, 1, 1, 1, MAX_VAL]); test_id += 1
    
    # Наборы с большими скачками, провоцирующие переполнение int внутри priority_queue
    write_test(test_id, 4, [2 * 10**9 // 3] * 4); test_id += 1
    
    for _ in range(3):
        n = random.randint(5, 10)
        pops = [random.randint(1, 100) for _ in range(n)]
        write_test(test_id, n, pops); test_id += 1

    # --- БЛОК 3: Тесты против переполнения int и багов с типами данных (20 тестов: 016-035) ---
    # Числа подобраны так, чтобы сумма промежуточных слияний была > 2^31 - 1
    write_test(test_id, 3, [1500000000, 1500000000, 1500000000]); test_id += 1
    
    # Тесты со значениями около границы INT_MAX
    write_test(test_id, 4, [2147483647, 2147483647, 10, 10]); test_id += 1
    
    # Интенсивные каверзные структуры средних размеров
    for _ in range(18):
        n = random.randint(20, 50)
        # Генерируем элементы так, чтобы при суммировании они быстро улетали за int
        pops = [random.randint(5 * 10**8, MAX_VAL) for _ in range(n)]
        write_test(test_id, n, pops); test_id += 1

    # --- БЛОК 4: Средние случайные тесты (40 тестов: 036-075) ---
    for _ in range(40):
        n = random.randint(500, 5000)
        pops = [random.randint(1, 10**7) for _ in range(n)]
        write_test(test_id, n, pops); test_id += 1

    # --- БЛОК 5: Большие стресс-тесты на максимальных ограничениях (45 тестов: 076-120) ---
    # N = 100 000 — проверка асимптотики O(N log N) и общего переполнения итогового ответа

    # 076-085: Все элементы равны 1 (Ответ помещается в int, но глубина дерева максимальна)
    for _ in range(10):
        n = random.randint(MAX_N - 5000, MAX_N)
        pops = [1] * n
        write_test(test_id, n, pops); test_id += 1

    # 086-095: Все элементы равны MAX_VAL (Максимально возможный ответ ~ 1.7 * 10^15)
    for _ in range(10):
        n = random.randint(MAX_N - 5000, MAX_N)
        pops = [MAX_VAL] * n
        write_test(test_id, n, pops); test_id += 1

    # 096-105: Полностью случайный разреженный шум по всему диапазону координат
    for _ in range(10):
        pops = [random.randint(1, MAX_VAL) for _ in range(MAX_N)]
        write_test(test_id, MAX_N, pops); test_id += 1

    # 106-115: Худший случай для линейных перерасчетов кучи (последовательность Фибоначчи или экспоненциальный рост)
    # Вынуждает дерево превращаться в бамбук
    for _ in range(10):
        pops = []
        curr = random.randint(1, 100)
        for _ in range(MAX_N):
            pops.append(curr)
            curr = min(MAX_VAL, curr + random.randint(1, 10))
        random.shuffle(pops)
        write_test(test_id, MAX_N, pops); test_id += 1

    # 116-120: Плотные кластеры одинаковых весов, разбросанные по диапазону
    for _ in range(5):
        pops = []
        for _ in range(5):
            center = random.randint(10**6, MAX_VAL - 1000)
            pops.extend([random.randint(center - 500, center + 500) for _ in range(MAX_N // 5)])
        random.shuffle(pops)
        write_test(test_id, len(pops), pops); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, pops):
    """Вспомогательная функция для записи теста в файл формата XXX.in с трехзначной нумерацией"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n}\n")
        f.write(" ".join(map(str, pops)) + "\n")

if __name__ == "__main__":
    random.seed(111213)
    generate_tests()