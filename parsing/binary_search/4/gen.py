#!/usr/bin/env python3
import random
import os
import math

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_C = 10**10

    # --- БЛОК 1: Примеры из условия (1-2) ---
    write_test(test_id, 2.0); test_id += 1
    write_test(test_id, 18.0); test_id += 1

    # --- БЛОК 2: Маленькие краевые кейсы (3-12) ---
    write_test(test_id, 1.0); test_id += 1                           # Минимальное возможное значение C (x = 0.6180339887...)
    write_test(test_id, 1.000001); test_id += 1                      # Чуть больше минимума
    write_test(test_id, 3.0); test_id += 1
    write_test(test_id, 4.0); test_id += 1
    
    # Красивые целые значения x, дающие точные значения C
    for target_x in [2.0, 3.0, 5.0, 9.0, 16.0, 25.0]:
        c_val = target_x**2 + math.sqrt(target_x)
        write_test(test_id, c_val); test_id += 1

    # --- БЛОК 3: Граничные значения и целые C (13-22) ---
    write_test(test_id, float(MAX_C)); test_id += 1                 # Ровно максимальное ограничение 10^10
    write_test(test_id, MAX_C - 0.123456); test_id += 1             # Чуть меньше максимума
    
    # Случайные небольшие значения C
    for _ in range(8):
        c_val = random.uniform(1.0, 100.0)
        write_test(test_id, c_val); test_id += 1

    # --- БЛОК 4: Дробные значения с высокой точностью знаков (23-35) ---
    # Генерируем C из случайных x с длинным хвостом, чтобы проверить точность бинпоиска
    for _ in range(13):
        target_x = random.uniform(0.7, 1000.0)
        c_val = target_x**2 + math.sqrt(target_x)
        write_test(test_id, c_val); test_id += 1

    # --- БЛОК 5: Средний диапазон значений (36-45) ---
    for _ in range(10):
        c_val = random.uniform(10**4, 10**7)
        write_test(test_id, c_val); test_id += 1

    # --- БЛОК 6: Большие и максимальные стресс-тесты (46-60) ---
    # Значения C вплотную подходят к 10^10. На таких тестах x достигает 10^5. 
    # Проверяется точность вещественных вычислений (double) и число итераций бинарного поиска (не менее 60-80).
    for _ in range(15):
        c_val = random.uniform(10**9, MAX_C)
        write_test(test_id, c_val); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, c_value):
    """Вспомогательная функция для записи вещественного числа в файл формата XX.in"""
    filename = f"tests/{str(test_id).zfill(2)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        # Записываем с 10 знаками после запятой для стабильности парсинга
        f.write(f"{c_value:.10f}\n")

if __name__ == "__main__":
    # Фиксируем seed для стабильной генерации пакета тестов
    random.seed(40404)
    generate_tests()