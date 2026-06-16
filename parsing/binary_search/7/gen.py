#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_N = 2 * 10**8

    # --- БЛОК 1: Примеры из условия (1-2) ---
    write_test(test_id, 4, 1, 1); test_id += 1
    write_test(test_id, 5, 1, 2); test_id += 1

    # --- БЛОК 2: Краевые и минимальные кейсы (3-15) ---
    write_test(test_id, 1, 1, 1); test_id += 1                     # Нужна ровно 1 копия (Ответ: min(x,y))
    write_test(test_id, 1, 1, 10); test_id += 1                    # Сильная разница в скорости при n=1
    write_test(test_id, 1, 10, 2); test_id += 1                    # Сильная разница в обратную сторону при n=1
    write_test(test_id, 2, 1, 1); test_id += 1                     # 2 копии, одинаковые быстрые ксероксы
    write_test(test_id, 2, 1, 10); test_id += 1                    # 2 копии, выгоднее сделать обе на первом ксероксе
    write_test(test_id, 2, 10, 1); test_id += 1                    # 2 копии, выгоднее сделать обе на втором ксероксе
    write_test(test_id, 3, 2, 3); test_id += 1                     # 3 копии, дробное соотношение скоростей
    write_test(test_id, 3, 5, 5); test_id += 1                     # 3 копии, одинаковые медленные ксероксы
    
    # Точечные стыки временных интервалов на маленьких n
    for x_speed in [1, 2, 3]:
        for y_speed in [1, 2, 3]:
            write_test(test_id, 10, x_speed, y_speed); test_id += 1

    # --- БЛОК 3: Тесты на кратность и идеальный баланс (16-25) ---
    # Ситуации, когда суммарное время работы идеально делится на скорости обоих ксероксов
    write_test(test_id, 100, 1, 1); test_id += 1
    write_test(test_id, 1000, 2, 4); test_id += 1                  # Скорости кратны друг другу (один в 2 раза быстрее)
    write_test(test_id, 5000, 3, 9); test_id += 1                  # Один в 3 раза быстрее
    write_test(test_id, 10000, 5, 5); test_id += 1
    
    # Случайные небольшие значения N
    for _ in range(6):
        n = random.randint(15, 200)
        x = random.randint(1, 10)
        y = random.randint(1, 10)
        write_test(test_id, n, x, y); test_id += 1

    # --- БЛОК 4: Асимметричные скорости на средних N (26-35) ---
    # Проверяет, как алгоритм распределяет нагрузку, когда один ксерокс работает на пределе (1 сек), 
    # а второй еле дышит (10 сек).
    for _ in range(5):
        n = random.randint(1000, 50000)
        write_test(test_id, n, 1, 10); test_id += 1
        write_test(test_id, n, 10, 1); test_id += 1

    # --- БЛОК 5: Средние и предмаксимальные случайные тесты (36-45) ---
    for _ in range(10):
        n = random.randint(10**5, 5 * 10**7)
        x = random.randint(1, 10)
        y = random.randint(1, 10)
        write_test(test_id, n, x, y); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (46-60) ---
    # Проверяет асимптотику бинарного поиска при N, вплотную приближающемся к 2 * 10^8.
    # Если правая граница `R` в бинпоиске зашита константой или вычислена неверно, тут вылетит Wrong Answer.
    
    # 46-50: Ровно максимальное ограничение N = 200 000 000
    for _ in range(5):
        x = random.randint(1, 10)
        y = random.randint(1, 10)
        write_test(test_id, MAX_N, x, y); test_id += 1

    # 51-55: Худший случай по времени (N максимальное, ксероксы самые медленные — по 10 секунд)
    # Ответ будет в районе 10^9 секунд, проверяет знаковые типы.
    for _ in range(5):
        n = random.randint(MAX_N - 1000, MAX_N)
        write_test(test_id, n, 10, 10); test_id += 1

    # 56-60: Самый быстрый случай на максимальном N (ксероксы по 1 секунде)
    for _ in range(5):
        n = random.randint(MAX_N - 1000, MAX_N)
        write_test(test_id, n, 1, 1); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, x, y):
    """Вспомогательная функция для записи теста в файл формата XX.in"""
    filename = f"tests/{str(test_id).zfill(2)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n} {x} {y}\n")

if __name__ == "__main__":
    # Фиксируем seed для полной детерминированности пакета тестов
    random.seed(777111)
    generate_tests()