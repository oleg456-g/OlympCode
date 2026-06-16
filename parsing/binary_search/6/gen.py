#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MIN_L = 100
    MAX_L = 10**7

    # --- БЛОК 1: Пример из условия (1) ---
    write_test(test_id, 4, 11, [802, 743, 457, 539]); test_id += 1

    # --- БЛОК 2: Краевые и минимальные кейсы (2-12) ---
    write_test(test_id, 1, 1, [500]); test_id += 1                 # 1 провод, 1 кусок (Ответ: 500)
    write_test(test_id, 1, 2, [500]); test_id += 1                 # 1 провод, деление ровно пополам (Ответ: 250)
    write_test(test_id, 1, 3, [500]); test_id += 1                 # Округление вниз при делении (500 / 3 = 166.66 -> Ответ: 166)
    write_test(test_id, 2, 1000, [100, 100]); test_id += 1         # Невозможно получить даже по 1 см (Ответ: 0)
    write_test(test_id, 5, 5, [100, 100, 100, 100, 100]); test_id += 1 # Ответ равен исходной длине провода (Ответ: 100)
    write_test(test_id, 3, 4, [100, 100, 100]); test_id += 1       # Чуть-чуть не хватает проводов (Ответ: 50)
    write_test(test_id, 1, 10000, [MAX_L]); test_id += 1           # 1 огромный провод пилится на максимум кусков
    write_test(test_id, 10, 1, [100]*10); test_id += 1             # Много проводов, нужен всего 1 кусок (Ответ: 100)
    write_test(test_id, 4, 5, [101, 101, 101, 101]); test_id += 1   # Проверка остатков (Ответ: 50)
    write_test(test_id, 2, 3, [200, 299]); test_id += 1            # Остатки отбрасываются (Ответ: 133 или около того)
    write_test(test_id, 5, 2, [100, 105, 200, 300, 110]); test_id += 1

    # --- БЛОК 3: Тесты с экстремальными и граничными значениями (13-22) ---
    write_test(test_id, 10000, 10000, [MIN_L]*10000); test_id += 1  # Все параметры на минималках (Ответ: 100)
    write_test(test_id, 10000, 1, [MAX_L]*10000); test_id += 1      # Максимальные провода, нужен 1 кусок (Ответ: MAX_L)
    write_test(test_id, 10000, 10000, [MAX_L]*10000); test_id += 1  # Максимальные провода и максимальный K (Ответ: MAX_L)
    
    # Тесты, где ответ гарантированно равен 1 см
    write_test(test_id, 10, 1000, [100]*10); test_id += 1           # 10*100 = 1000 кусков по 1 см (Ответ: 1)
    write_test(test_id, 10, 1001, [100]*10); test_id += 1           # 1000 кусков есть, а 1001 уже нельзя (Ответ: 0)

    # Несколько случайных тестов в максимальных границах длин проводов
    for _ in range(5):
        n = random.randint(10, 50)
        k = random.randint(5, 20)
        arr = [random.randint(MAX_L - 1000, MAX_L) for _ in range(n)]
        write_test(test_id, n, k, arr); test_id += 1

    # --- БЛОК 4: Тесты со специальной структурой и кратностью (23-35) ---
    # Все провода делятся без остатка на целевой ответ (проверка строгих неравенств в бинпоиске)
    for target_ans in [120, 500, 2500, 10000, 123456]:
        n = random.randint(50, 100)
        arr = [target_ans * random.randint(1, 10) for _ in range(n)]
        # Считаем сколько кусков суммарно выйдет
        total_k = sum(x // target_ans for x in arr)
        write_test(test_id, n, total_k, arr); test_id += 1
        # Тот же массив, но просим на 1 кусок больше -> ответ должен стать меньше target_ans
        write_test(test_id, n, total_k + 1, arr); test_id += 1

    # Провода с "неудобными" остатками (длина вида target * M + (target - 1))
    for _ in range(3):
        target_ans = random.randint(1000, 5000)
        n = random.randint(20, 50)
        arr = [target_ans * random.randint(2, 5) + (target_ans - 1) for _ in range(n)]
        total_k = sum(x // target_ans for x in arr)
        write_test(test_id, n, total_k, arr); test_id += 1

    # --- БЛОК 5: Средние случайные тестов (36-45) ---
    for _ in range(10):
        n = random.randint(500, 2000)
        k = random.randint(500, 2000)
        arr = [random.randint(MIN_L, 10**5) for _ in range(n)]
        write_test(test_id, n, k, arr); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (46-60) ---
    # N = 10 000, K = 10 000, L_i до 10^7.
    # Проверяет эффективность функции проверки (check) в бинпоиске. 
    # Суммарное число операций должно быть порядка O(N * log(MAX_L)) -> 10000 * 24 ≈ 2.4 * 10^5 действий, что выполняется мгновенно.
    max_nk = 10000
    
    # 46-50: Полностью случайные большие тесты, где K сопоставимо с N
    for _ in range(5):
        n = max_nk
        k = random.randint(5000, max_nk)
        arr = [random.randint(MIN_L, MAX_L) for _ in range(n)]
        write_test(test_id, n, k, arr); test_id += 1

    # 51-55: Сильный дефицит кусков (K очень большое, ответ будет маленьким)
    for _ in range(5):
        n = max_nk
        k = max_nk
        # Провода относительно небольшие, а просят очень много кусков
        arr = [random.randint(MIN_L, MIN_L * 5) for _ in range(n)]
        write_test(test_id, n, k, arr); test_id += 1

    # 56-60: Обилие ресурсов (K маленькое, провода огромные, ответ будет очень большим)
    for _ in range(5):
        n = max_nk
        k = random.randint(1, 10)
        arr = [random.randint(MAX_L // 2, MAX_L) for _ in range(n)]
        write_test(test_id, n, k, arr); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, k, lines):
    """Вспомогательная функция для записи теста в файл формата XX.in"""
    filename = f"tests/{str(test_id).zfill(2)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n} {k}\n")
        for line in lines:
            f.write(f"{line}\n")

if __name__ == "__main__":
    # Фиксируем seed для стабильности генерации
    random.seed(606060)
    generate_tests()