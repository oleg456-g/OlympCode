#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_N = 400
    MAX_K = 800

    # --- БЛОК 1: Пример из условия (1 тест: 001) ---
    write_test(test_id, 7, [8, 7, 4, 5, 3, 4, 2], 9, [
        (2, 5), (1, 8), (2, 9), (2, 4), (1, 100), (2, 13), (2, 10), (1, 11), (1, 14)
    ]); test_id += 1

    # --- БЛОК 2: Краевые и вырожденные кейсы (14 тестов: 002-015) ---
    write_test(test_id, 0, [], 5, [(1, 10), (2, 20)]); test_id += 1  # 0 спортсменов
    write_test(test_id, 5, [10, 20, 30, 40, 50], 0, []); test_id += 1 # 0 костюмов
    write_test(test_id, 0, [], 0, []); test_id += 1                  # И спортсменов, и костюмов 0
    
    # Костюмов ровно столько же, сколько спортсменов
    write_test(test_id, 3, [100, 100, 100], 3, [(1, 50), (1, 50), (1, 50)]); test_id += 1
    write_test(test_id, 3, [10, 20, 30], 3, [(2, 5), (2, 15), (2, 25)]); test_id += 1
    
    # Только один тип костюмов (только плавки или только спецназ)
    write_test(test_id, 4, [10, 50, 100, 500], 3, [(1, 10), (1, 20), (1, 30)]); test_id += 1
    write_test(test_id, 4, [10, 50, 100, 500], 3, [(2, 10), (2, 20), (2, 30)]); test_id += 1
    
    # Костюмов гораздо больше, чем спортсменов (проверка отсечения лишних)
    write_test(test_id, 2, [10, 20], 6, [(1, 100), (1, 200), (2, 5), (2, 10), (2, 15), (2, 20)]); test_id += 1
    
    # Один супер-спортсмен и много мелких костюмов
    write_test(test_id, 1, [10000], 5, [(1, 10), (2, 50), (1, 5), (2, 5), (1, 300)]); test_id += 1
    
    # Все параметры минимальны (скорость 1, мощность 1)
    write_test(test_id, 3, [1, 1, 1], 3, [(1, 1), (2, 1), (1, 1)]); test_id += 1
    
    # Все параметры максимальны
    write_test(test_id, 2, [10000, 10000], 2, [(1, 300), (2, 300)]); test_id += 1

    for _ in range(3):
        n = random.randint(5, 15)
        k = random.randint(5, 15)
        speeds = [random.randint(10, 100) for _ in range(n)]
        suits = [(random.randint(1, 2), random.randint(1, 100)) for _ in range(k)]
        write_test(test_id, n, speeds, k, suits); test_id += 1

    # --- БЛОК 3: Тесты на проверку правильности жадного выбора (25 тестов: 016-040) ---
    # Ситуации, где плавки выгоднее процентов и наоборот (для мелких/крупных скоростей)
    # Спортсмен со скоростью 10 и костюм 100% дают +10. Плавки +15 дают больше, хотя процент большой.
    write_test(test_id, 2, [10, 1000], 2, [(1, 100), (2, 50)]); test_id += 1
    
    # Интенсивные тесты средних размеров с каверзным сочетанием весов
    for _ in range(24):
        n = random.randint(20, 40)
        k = random.randint(20, 60)
        # Половина скоростей маленькие, половина огромные
        speeds = [random.randint(1, 20) for _ in range(n // 2)] + [random.randint(5000, 10000) for _ in range(n - n // 2)]
        random.shuffle(speeds)
        suits = []
        for _ in range(k):
            t = random.randint(1, 2)
            # Большие проценты (до 300) противопоставляются умеренным плавкам
            w = random.randint(200, 300) if t == 1 else random.randint(10, 150)
            suits.append((t, w))
        write_test(test_id, n, speeds, k, suits); test_id += 1

    # --- БЛОК 4: Средние случайные тесты (40 тестов: 041-080) ---
    for _ in range(40):
        n = random.randint(50, 150)
        k = random.randint(50, 250)
        speeds = [random.randint(1, 5000) for _ in range(n)]
        suits = [(random.randint(1, 2), random.randint(1, 300)) for _ in range(k)]
        write_test(test_id, n, speeds, k, suits); test_id += 1

    # --- БЛОК 5: Максимальные стресс-тесты (40 тестов: 081-120) ---
    # N = 400, K = 800 — верхние границы задачи

    # 081-090: Все костюмы — плавки (тип 2) максимальной мощности 300
    for _ in range(10):
        n = random.randint(MAX_N - 20, MAX_N)
        k = random.randint(MAX_K - 50, MAX_K)
        speeds = [random.randint(1, 1000) for _ in range(n)]
        suits = [(2, 300) for _ in range(k)]
        write_test(test_id, n, speeds, k, suits); test_id += 1

    # 091-100: Все костюмы — проценты (тип 1) максимальной мощности 300%
    for _ in range(10):
        n = random.randint(MAX_N - 20, MAX_N)
        k = random.randint(MAX_K - 50, MAX_K)
        speeds = [random.randint(5000, 10000) for _ in range(n)]
        suits = [(1, 300) for _ in range(k)]
        write_test(test_id, n, speeds, k, suits); test_id += 1

    # 101-110: Худший случай — хаотичный шум на полных лимитах
    for _ in range(10):
        speeds = [random.randint(1, 10000) for _ in range(MAX_N)]
        suits = [(random.randint(1, 2), random.randint(1, 300)) for _ in range(MAX_K)]
        write_test(test_id, MAX_N, speeds, MAX_K, suits); test_id += 1

    # 111-120: Костюмов сильно меньше, чем спортсменов (N = 400, K = 50)
    for _ in range(10):
        k_val = random.randint(30, 60)
        speeds = [random.randint(1, 10000) for _ in range(MAX_N)]
        suits = [(random.randint(1, 2), random.randint(1, 300)) for _ in range(k_val)]
        write_test(test_id, MAX_N, speeds, k_val, suits); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, speeds, k, suits):
    """Вспомогательная функция для записи теста в файл формата XXX.in"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n}\n")
        if n > 0:
            f.write(" ".join(map(str, speeds)) + "\n")
        else:
            f.write("\n")
        f.write(f"{k}\n")
        for t, w in suits:
            f.write(f"{t} {w}\n")

if __name__ == "__main__":
    random.seed(606060)
    generate_tests()