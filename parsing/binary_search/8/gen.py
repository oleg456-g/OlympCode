#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_COORD = 10**9

    # --- БЛОК 1: Пример из условия (1) ---
    write_test(test_id, 6, 3, [2, 5, 7, 11, 15, 20]); test_id += 1

    # --- БЛОК 2: Минимальные и краевые кейсы (2-12) ---
    write_test(test_id, 3, 2, [1, 10, 100]); test_id += 1            # 2 коровы (Ответ: крайние стойла, 100 - 1 = 99)
    write_test(test_id, 4, 3, [1, 2, 3, 4]); test_id += 1            # Стойла идут подряд (Ответ: 1)
    write_test(test_id, 5, 2, [0, 10, 20, 30, 40]); test_id += 1     # Коровы по краям (Ответ: 40)
    write_test(test_id, 5, 5, [1, 3, 5, 7, 9]); test_id += 1         # Коров столько же, сколько стойл (Ответ: мин. шаг = 2)
    write_test(test_id, 4, 3, [1, 100, 101, 200]); test_id += 1      # Большой разрыв посередине
    write_test(test_id, 6, 4, [1, 2, 10, 11, 20, 21]); test_id += 1  # Парные стойла близко друг к другу
    
    # Стойла на равных расстояниях
    write_test(test_id, 10, 3, [i * 10 for i in range(10)]); test_id += 1
    write_test(test_id, 10, 5, [i * 10 for i in range(10)]); test_id += 1
    write_test(test_id, 5, 3, [1, 100, 101, 102, 200]); test_id += 1
    write_test(test_id, 4, 2, [1, MAX_COORD]); test_id += 1          # Максимальное расстояние при N=4, K=2

    # --- БЛОК 3: Тесты на точные границы (off-by-one ошибки) (13-22) ---
    # Специально создаем структуры, где ответ равен ровно какому-то числу, 
    # и проверяем поведение check(mid) и check(mid + 1)
    for target in [5, 50, 1000, 12345]:
        coords = [0, target, target * 2, target * 3 - 1, target * 4]
        write_test(test_id, 5, 4, coords); test_id += 1
        
    for _ in range(6):
        n = random.randint(10, 30)
        k = random.randint(3, 5)
        coords = sorted(random.sample(range(1, 1000), n))
        write_test(test_id, n, k, coords); test_id += 1

    # --- БЛОК 4: Асимметричные распределения и кластеры (23-35) ---
    # Большое скопление стойл в начале и единичные стойла в конце
    for _ in range(6):
        n = 100
        k = random.randint(5, 15)
        coords = sorted([random.randint(1, 500) for _ in range(80)] + [random.randint(50000, 100000) for _ in range(20)])
        write_test(test_id, n, k, coords); test_id += 1

    # Большое скопление в конце и пустота в начале
    for _ in range(7):
        n = 100
        k = random.randint(5, 15)
        coords = sorted([random.randint(1, 1000) for _ in range(10)] + [random.randint(900000, 1000000) for _ in range(90)])
        write_test(test_id, n, k, coords); test_id += 1

    # --- БЛОК 5: Средние случайные тесты (36-45) ---
    for _ in range(10):
        n = random.randint(1000, 3000)
        k = random.randint(100, 500)
        coords = sorted(random.sample(range(1, 10**7), n))
        write_test(test_id, n, k, coords); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (46-60) ---
    # N = 10 000, координаты до 10^9. Проверка асимптотики бинпоиска за O(N log(MAX_COORD)) 
    # в рамках жесткого лимита времени 300 мс.
    max_n = 10000
    
    # 46-50: Случайные тесты на полном диапазоне с малым количеством коров K
    for _ in range(5):
        k = random.randint(3, 10)
        coords = sorted(random.sample(range(1, MAX_COORD), max_n))
        write_test(test_id, max_n, k, coords); test_id += 1

    # 51-55: Случайные тесты на полном диапазоне с большим количеством коров K (около N/2)
    for _ in range(5):
        k = random.randint(4000, 5000)
        coords = sorted(random.sample(range(1, MAX_COORD), max_n))
        write_test(test_id, max_n, k, coords); test_id += 1

    # 56-60: Очень плотные стойла (идут подряд с небольшим шумом) на огромных координатах
    for _ in range(5):
        k = random.randint(1000, 2000)
        start = random.randint(1, MAX_COORD - 20000)
        coords = sorted(list(set([start + i + random.randint(0, 1) for i in range(max_n * 2)])))[:max_n]
        write_test(test_id, max_n, k, coords); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, k, coords):
    """Вспомогательная функция для записи теста в файл формата XX.in"""
    filename = f"tests/{str(test_id).zfill(2)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n} {k}\n")
        f.write(" ".join(map(str, coords)) + "\n")

if __name__ == "__main__":
    # Фиксируем seed для стабильной генерации пакета тестов
    random.seed(808080)
    generate_tests()