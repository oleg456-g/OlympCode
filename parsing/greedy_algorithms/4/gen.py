#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_VAL = 1000

    # --- БЛОК 1: Примеры из условия (3 теста: 001-003) ---
    write_test(test_id, 100, 20, 1, [50]); test_id += 1
    write_test(test_id, 100, 100, 3, [10, 20, 80]); test_id += 1
    write_test(test_id, 100, 50, 1, [50]); test_id += 1

    # --- БЛОК 2: Минимальные и краевые кейсы (17 тестов: 004-020) ---
    write_test(test_id, 10, 10, 0, []); test_id += 1                 # 0 заправок, бака ровно хватает до финиша
    write_test(test_id, 10, 9, 0, []); test_id += 1                  # 0 заправок, бака не хватает до финиша -> -1
    write_test(test_id, 100, 50, 2, [30, 60]); test_id += 1          # Заправки с запасом
    write_test(test_id, 100, 50, 2, [50, 100]); test_id += 1         # Заправка на финише и ровно посередине
    write_test(test_id, 1000, 1000, 5, [100, 200, 300, 400, 500]); test_id += 1 # Бака хватает на весь путь, заправки не нужны
    
    # Расстояние до первой заправки ровно k, и от нее до финиша ровно k
    write_test(test_id, 60, 30, 1, [30]); test_id += 1
    
    # Невозможно доехать от последней заправки до финиша
    write_test(test_id, 100, 40, 2, [30, 50]); test_id += 1
    
    # Невозможно доехать между заправками в середине пути
    write_test(test_id, 200, 50, 3, [40, 100, 140]); test_id += 1
    
    # Много заправок в одной точке или очень близко
    write_test(test_id, 50, 20, 4, [10, 11, 12, 30]); test_id += 1
    
    # Заправки стоят прямо на старте (в точке 0 не должно быть, но проверим малые значения)
    write_test(test_id, 100, 40, 2, [1, 41]); test_id += 1
    
    for _ in range(7):
        n = random.randint(20, 50)
        k = random.randint(5, 15)
        s = random.randint(1, 5)
        stations = sorted(random.sample(range(1, n), s))
        write_test(test_id, n, k, s, stations); test_id += 1

    # --- БЛОК 3: Тесты на жадный выбор и ловушки "лишних" остановок (20 тестов: 021-040) ---
    # Проверка того, что алгоритм не заправляется на каждой станции подряд, а едет до предела
    write_test(test_id, 100, 30, 5, [10, 20, 30, 60, 90]); test_id += 1
    write_test(test_id, 100, 40, 6, [10, 20, 30, 40, 70, 80]); test_id += 1
    
    # Серии тестов со случайными плотными группами бензоколонок
    for _ in range(18):
        n = random.randint(100, 300)
        k = random.randint(30, 60)
        # Генерируем цепочку, где можно либо оптимально заправиться, либо ошибиться
        stations = []
        curr = 0
        while curr < n:
            curr += random.randint(5, k)
            if curr < n:
                stations.append(curr)
                # Добавим «мусорную» заправку рядом для провокации лишнего шага
                if curr + 2 < n and random.choice([True, False]):
                    stations.append(curr + 2)
        stations = sorted(list(set(stations)))
        write_test(test_id, n, k, len(stations), stations); test_id += 1

    # --- БЛОК 4: Средние случайные тесты (40 тестов: 041-080) ---
    for _ in range(40):
        n = random.randint(200, 700)
        k = random.randint(20, 100)
        s = random.randint(5, 30)
        stations = sorted(random.sample(range(1, n), min(s, n - 1)))
        write_test(test_id, n, k, len(stations), stations); test_id += 1

    # --- БЛОК 5: Большие стресс-тесты на максимальных ограничениях (40 тестов: 081-120) ---
    # Ограничения N, K <= 1000

    # 081-090: Худший случай — заправка требуется на каждом шаге (цепочка ровно через k километров)
    for _ in range(10):
        k = random.randint(10, 25)
        stations = [i * k for i in range(1, 1000 // k)]
        n = stations[-1] + random.randint(1, k)
        if n > MAX_VAL:
            n = MAX_VAL
        write_test(test_id, n, k, len(stations), stations); test_id += 1

    # 091-100: Большое количество заправок (максимально возможная плотность)
    for _ in range(10):
        n = MAX_VAL
        k = random.randint(50, 150)
        # Заполняем почти каждый километр заправками
        s = random.randint(300, 500)
        stations = sorted(random.sample(range(1, n), s))
        write_test(test_id, n, k, len(stations), stations); test_id += 1

    # 101-110: Полностью случайные тесты на максимальных границах (гарантированно проходимые)
    for _ in range(10):
        n = MAX_VAL
        k = random.randint(100, 200)
        # Строим заведомо проходимый путь
        stations = []
        curr = 0
        while curr < n - k:
            curr += random.randint(k // 2, k)
            if curr < n:
                stations.append(curr)
        # Добавляем случайный шум
        extra = random.sample(range(1, n), min(50, n - len(stations) - 1))
        stations = sorted(list(set(stations + extra)))
        write_test(test_id, n, k, len(stations), stations); test_id += 1

    # 111-120: Полностью случайные тесты на максимальных границах (часто непроходимые -> ответ -1)
    for _ in range(10):
        n = MAX_VAL
        k = random.randint(10, 50) # Маленький бак при большом расстоянии
        s = random.randint(10, 40)
        stations = sorted(random.sample(range(1, n), s))
        write_test(test_id, n, k, len(stations), stations); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, k, s, stations):
    """Вспомогательная функция для записи теста в файл формата XXX.in с трехзначной нумерацией"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n} {k}\n")
        f.write(f"{s}\n")
        if s > 0:
            f.write(" ".join(map(str, stations)) + "\n")
        else:
            f.write("\n")

if __name__ == "__main__":
    random.seed(777888)
    generate_tests()