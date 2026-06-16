#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_M = 15000
    MAX_N = 1000

    # Вспомогательная функция для генерации строки параметров помощника
    def gen_helper(t_range, z_range, y_range):
        return [random.randint(*t_range), random.randint(*z_range), random.randint(*y_range)]

    # --- БЛОК 1: Примеры из условия (1-2) ---
    write_test(test_id, 1, 2, [[2, 1, 1], [1, 1, 2]]); test_id += 1
    write_test(test_id, 2, 2, [[1, 1, 1], [1, 1, 1]]); test_id += 1

    # --- БЛОК 2: Краевые случаи и вырожденные значения (3-15) ---
    write_test(test_id, 0, 1, [[5, 2, 10]]); test_id += 1             # M = 0, 1 помощник (Ответ: 0 и 0)
    write_test(test_id, 0, 5, [[1, 1, 1] for _ in range(5)]); test_id += 1 # M = 0, много помощников
    write_test(test_id, 10, 1, [[2, 5, 10]]); test_id += 1            # Ровно 1 помощник, делает серии по 5
    write_test(test_id, 5, 1, [[3, 5, 100]]); test_id += 1            # Закончил ровно перед отдыхом
    write_test(test_id, 6, 1, [[3, 5, 10]]); test_id += 1             # Захватывает ровно 1 период отдыха
    write_test(test_id, 1, 1000, [[100, 1000, 100] for _ in range(1000)]); test_id += 1 # 1 шарик на 1000 медленных людей
    write_test(test_id, 15000, 1, [[1, 1000, 1]]); test_id += 1       # 1 супер-помощник фигачит всё сам
    
    # Все помощники одинаковые
    write_test(test_id, 100, 10, [[2, 2, 5] for _ in range(10)]); test_id += 1
    # Отдых практически отсутствует (Y = 1, Z = 1000)
    write_test(test_id, 50, 2, [[2, 1000, 1], [3, 1000, 1]]); test_id += 1
    # Очень долгий отдых (Y = 100, Z = 1)
    write_test(test_id, 5, 5, [[1, 1, 100] for _ in range(5)]); test_id += 1

    # --- БЛОК 3: Тесты со случайными небольшими параметрами (16-25) ---
    for _ in range(10):
        m = random.randint(10, 100)
        n = random.randint(2, 10)
        helpers = [gen_helper((1, 10), (1, 5), (1, 10)) for _ in range(n)]
        write_test(test_id, m, n, helpers); test_id += 1

    # --- БЛОК 4: Тесты на "избыточность" на последней минуте (26-35) ---
    # Случаи, когда на оптимальной минуте T помощники суммарно могут надуть 
    # гораздо больше, чем M. Проверяет корректность жадного уменьшения остатка.
    for _ in range(10):
        m = random.randint(100, 500)
        n = random.randint(50, 100)  # Людей много, каждый надует по чуть-чуть
        helpers = [gen_helper((1, 5), (1, 10), (1, 5)) for _ in range(n)]
        write_test(test_id, m, n, helpers); test_id += 1

    # --- БЛОК 5: Средний масштаб (36-45) ---
    for _ in range(10):
        m = random.randint(1000, 5000)
        n = random.randint(100, 300)
        helpers = [gen_helper((1, 50), (1, 100), (1, 50)) for _ in range(n)]
        write_test(test_id, m, n, helpers); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (46-60) ---
    # Ровно M = 15000, N = 1000. Коэффициенты T_i, Y_i до 100.
    # Проверяет эффективность бинпоиска. Верхняя граница поиска R должна 
    # составлять не менее 15000 * 100 + отдых ≈ 1.5 * 10^6 минут.
    
    # 46-50: Полный хаос (все параметры рандомные на максимумах)
    for _ in range(5):
        helpers = [gen_helper((1, 100), (1, 1000), (1, 100)) for _ in range(MAX_N)]
        write_test(test_id, MAX_M, MAX_N, helpers); test_id += 1

    # 51-55: Все очень медленные, а шариков максимум (худший случай для времени T)
    for _ in range(5):
        helpers = [gen_helper((80, 100), (1, 5), (80, 100)) for _ in range(MAX_N)]
        write_test(test_id, MAX_M, MAX_N, helpers); test_id += 1

    # 56-60: Все супер-быстрые, шариков максимум (лучший случай для времени T, высокая плотность)
    for _ in range(5):
        helpers = [gen_helper((1, 2), (500, 1000), (1, 2)) for _ in range(MAX_N)]
        write_test(test_id, MAX_M, MAX_N, helpers); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, m, n, helpers):
    """Вспомогательная функция для записи теста в файл формата XX.in"""
    filename = f"tests/{str(test_id).zfill(2)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{m} {n}\n")
        for h in helpers:
            f.write(f"{h[0]} {h[1]} {h[2]}\n")

if __name__ == "__main__":
    # Фиксируем seed для стабильной генерации пакета тестов
    random.seed(1010101)
    generate_tests()