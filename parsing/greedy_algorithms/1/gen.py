#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_TIME = 1440

    # --- БЛОК 1: Примеры из условия (2 теста: 001-002) ---
    write_test(test_id, 1, [[5, 10]]); test_id += 1
    write_test(test_id, 3, [[1, 5], [2, 3], [3, 4]]); test_id += 1

    # --- БЛОК 2: Минимальные и краевые кейсы (10 тестов: 003-012) ---
    write_test(test_id, 1, [[1, 1440]]); test_id += 1              # Одна заявка на весь день
    write_test(test_id, 2, [[1, 2], [2, 3]]); test_id += 1          # Две стыкующиеся заявки
    write_test(test_id, 2, [[1, 3], [2, 3]]); test_id += 1          # Пересекающиеся с одинаковым концом
    write_test(test_id, 2, [[1, 3], [1, 2]]); test_id += 1          # Пересекающиеся с одинаковым началом
    write_test(test_id, 3, [[10, 20], [10, 20], [10, 20]]); test_id += 1  # Полные дубликаты
    write_test(test_id, 2, [[1, 10], [5, 6]]); test_id += 1         # Вложенные заявки
    write_test(test_id, 4, [[1, 5], [5, 10], [10, 15], [15, 20]]); test_id += 1 # Цепочка подряд
    write_test(test_id, 2, [[1, 2], [1439, 1440]]); test_id += 1    # Границы суток
    write_test(test_id, 5, [[10, 20], [30, 40], [50, 60], [70, 80], [90, 100]]); test_id += 1 # Полностью независимые
    write_test(test_id, 3, [[1, 10], [5, 15], [2, 3]]); test_id += 1 # Хаотичные пересечения

    # --- БЛОК 3: Тестов на ловушки жадного алгоритма (23 теста: 013-035) ---
    write_test(test_id, 3, [[1, 10], [2, 3], [3, 4]]); test_id += 1 # Ловушка 1: сортировка по началу
    write_test(test_id, 3, [[1, 4], [3, 6], [5, 8]]); test_id += 1  # Ловушка 2: сортировка по длине
    
    # Ловушка 3: Множество пересечений в одной точке (звезда)
    star_requests = [[10, 20]] + [[15, 16] for _ in range(9)]
    write_test(test_id, len(star_requests), star_requests); test_id += 1

    # Структуры, где одна длинная заявка перекрывает много мелких (3 теста)
    for size in [5, 10, 15]:
        reqs = [[1, 100]] + [[i, i+1] for i in range(2, 2 + size - 1)]
        write_test(test_id, len(reqs), reqs); test_id += 1

    # Заявки типа "вилка"
    fork_reqs = [[1, 10], [10, 20], [20, 30], [1, 15], [15, 30]]
    write_test(test_id, len(fork_reqs), fork_reqs); test_id += 1

    # Сгенерированные каверзные пересечения (16 тестов)
    for _ in range(16):
        reqs = []
        for _ in range(15):
            s = random.randint(1, 100)
            f = s + random.randint(1, 30)
            reqs.append([s, f])
        write_test(test_id, len(reqs), reqs); test_id += 1

    # --- БЛОК 4: Средние случайные тесты (40 тестов: 036-075) ---
    for _ in range(40):
        n = random.randint(50, 200)
        reqs = []
        for _ in range(n):
            s = random.randint(1, MAX_TIME - 1)
            f = random.randint(s + 1, min(s + 120, MAX_TIME))
            reqs.append([s, f])
        write_test(test_id, n, reqs); test_id += 1

    # --- БЛОК 5: Большие стресс-тесты (45 тестов: 076-120) ---
    max_n = 1000

    # 076-090: Очень высокая плотность заявок (15 тестов)
    for _ in range(15):
        reqs = []
        for _ in range(max_n):
            s = random.randint(1, 200)
            f = s + random.randint(1, 20)
            reqs.append([s, f])
        write_test(test_id, max_n, reqs); test_id += 1

    # 091-105: Равномерное распределение по всему дню (15 тестов)
    for _ in range(15):
        reqs = []
        for _ in range(max_n):
            s = random.randint(1, MAX_TIME - 1)
            f = random.randint(s + 1, MAX_TIME)
            reqs.append([s, f])
        write_test(test_id, max_n, reqs); test_id += 1

    # 106-120: Все лекции имеют одинаковое время окончания или начала (15 тестов)
    for i in range(15):
        reqs = []
        fixed_end = random.randint(500, 1440)
        fixed_start = random.randint(1, 500)
        for j in range(max_n):
            if i % 2 == 0:
                s = random.randint(1, fixed_end - 1)
                reqs.append([s, fixed_end])
            else:
                f = random.randint(fixed_start + 1, MAX_TIME)
                reqs.append([fixed_start, f])
        write_test(test_id, max_n, reqs); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, reqs):
    """Вспомогательная функция для записи теста в файл формата XXX.in с трехзначной нумерацией"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n}\n")
        for start, end in reqs:
            f.write(f"{start} {end}\n")

if __name__ == "__main__":
    random.seed(424242)
    generate_tests()