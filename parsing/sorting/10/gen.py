#!/usr/bin/env python3
import random
import os

def generate_tests():
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_VAL = 10**9

    # --- БЛОК 1: Пример из условия (1) ---
    write_test(test_id, 3, 2, [5, 10, 7, 5, 5, 5], 6, [9, 5, 3, 5, 12, 10]); test_id += 1

    # --- БЛОК 2: Минимальные и тривиальные кейсы (2-15) ---
    write_test(test_id, 1, 1, [10], 1, [5]); test_id += 1           # 1 заказ, 1 дрон подходит
    write_test(test_id, 1, 1, [5], 1, [10]); test_id += 1           # 1 заказ, 1 дрон слишком жадный
    write_test(test_id, 1, 1, [MAX_VAL], 1, [MAX_VAL]); test_id += 1 # Граничные максимальные значения
    
    # Все дроны идеальные / все дроны слишком жадные
    write_test(test_id, 2, 2, [10, 20, 30, 40], 3, [5, 5, 5]); test_id += 1
    write_test(test_id, 2, 2, [10, 20, 30, 40], 3, [50, 100, 500]); test_id += 1
    
    # Огромные дисбалансы в размерах (1 заказ и куча дронов / куча заказов и 1 дрон)
    write_test(test_id, 1, 1, [50], 10, [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]); test_id += 1
    write_test(test_id, 4, 4, [i for i in range(1, 17)], 1, [10]); test_id += 1
    
    # Полностью одинаковые массивы
    write_test(test_id, 3, 3, [42]*9, 5, [42]*5); test_id += 1
    write_test(test_id, 2, 3, [10]*6, 10, [11]*10); test_id += 1
    
    # Добиваем блок маленькими случайными тестами
    for _ in range(5):
        n, m = random.randint(1, 3), random.randint(1, 3)
        costs = [random.randint(1, 100) for _ in range(n*m)]
        k = random.randint(1, 5)
        drones = [random.randint(1, 100) for _ in range(k)]
        write_test(test_id, n, m, costs, k, drones); test_id += 1

    # --- БЛОК 3: Тесты на точные совпадения границ (16-40) ---
    for target in [10, 500, 10000, 999999, MAX_VAL]:
        # Часть совпадает до единицы, часть на 1 меньше/больше
        write_test(test_id, 2, 2, [target-1, target, target+1, target*2], 3, [target, target, target+2]); test_id += 1
    
    for _ in range(20):
        n = random.randint(2, 5)
        m = random.randint(2, 5)
        k = random.randint(5, 20)
        # Генерируем из одного пула, чтобы было много точных совпадений cost == drone
        pool = [random.randint(1, 1000) for _ in range(10)]
        costs = [random.choice(pool) for _ in range(n*m)]
        drones = [random.choice(pool) for _ in range(k)]
        write_test(test_id, n, m, costs, k, drones); test_id += 1

    # --- БЛОК 4: Асимметричные распределения значений (41-70) ---
    # Очень дорогие заказы и дешевые дроны
    for _ in range(15):
        n, m = random.randint(5, 10), random.randint(5, 10)
        costs = [random.randint(10**6, 10**9) for _ in range(n*m)]
        k = random.randint(20, 50)
        drones = [random.randint(1, 10**4) for _ in range(k)]
        write_test(test_id, n, m, costs, k, drones); test_id += 1

    # Наоборот: очень дешевые заказы и заоблачно жадные дроны
    for _ in range(15):
        n, m = random.randint(5, 10), random.randint(5, 10)
        costs = [random.randint(1, 10**3) for _ in range(n*m)]
        k = random.randint(20, 50)
        drones = [random.randint(10**5, 10**9) for _ in range(k)]
        write_test(test_id, n, m, costs, k, drones); test_id += 1

    # --- БЛОК 5: Средние случайные тесты (71-100) ---
    for _ in range(30):
        n = random.randint(10, 25)
        m = random.randint(10, 25)
        costs = [random.randint(1, 10**8) for _ in range(n*m)]
        k = random.randint(100, 500)
        drones = [random.randint(1, 10**8) for _ in range(k)]
        write_test(test_id, n, m, costs, k, drones); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (101-120) ---
    # По спецификации: n, m <= 40 (макс 1600 заказов), k <= 2000
    for _ in range(20):
        n, m = 40, 40
        costs = [random.randint(1, MAX_VAL) for _ in range(1600)]
        k = 2000
        drones = [random.randint(1, MAX_VAL) for _ in range(k)]
        write_test(test_id, n, m, costs, k, drones); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, m, costs, k, drones):
    """Вспомогательная функция для форматированной записи теста"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n} {m}\n")
        # Записываем матрицу n строк по m элементов
        for i in range(n):
            row = costs[i*m : (i+1)*m]
            f.write(" ".join(map(str, row)) + "\n")
        f.write(f"{k}\n")
        f.write(" ".join(map(str, drones)) + "\n")

if __name__ == "__main__":
    random.seed(424242)
    generate_tests()