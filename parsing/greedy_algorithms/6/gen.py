#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_M = 10**9
    MAX_A = 10**9

    # --- БЛОК 1: Пример из условия (1 тест: 001) ---
    example_a = [1, 1, 10] + [1] * 28
    write_test(test_id, 11, example_a); test_id += 1

    # --- БЛОК 2: Минимальные и краевые кейсы (19 тестов: 002-020) ---
    write_test(test_id, 1, [1] * 31); test_id += 1                  # m = 1, все карточки дают по 1 секунде
    write_test(test_id, 1, [MAX_A] * 31); test_id += 1              # m = 1, все карточки дают максимум секунд
    write_test(test_id, MAX_M, [1] * 31); test_id += 1              # Нужен максимум секунд, карточки по 1 секунде
    write_test(test_id, MAX_M, [MAX_A] * 31); test_id += 1          # Максимальный m, максимальные карточки
    
    # Карточка с ценой 1 (индекс 0) дает гигантское время, остальные бесполезны
    write_test(test_id, 1000, [100000] + [1] * 30); test_id += 1
    
    # Карточка с максимальной стоимостью 2^30 дает много секунд, остальные по 1
    large_a = [1] * 30 + [MAX_A]
    write_test(test_id, 500000, large_a); test_id += 1

    # Степени двойки для секунд (прямая пропорциональность стоимости)
    geom_a = [2**i for i in range(31)]
    write_test(test_id, 1023, geom_a); test_id += 1
    write_test(test_id, 1024, geom_a); test_id += 1
    
    # Обратная пропорциональность (чем дороже карта, тем она менее выгодна)
    inv_geom_a = [2**(30-i) for i in range(31)]
    write_test(test_id, 100, inv_geom_a); test_id += 1

    # Рандомные мелкие краевые тесты
    for _ in range(10):
        m = random.randint(1, 100)
        a_arr = [random.randint(1, 5) for _ in range(31)]
        write_test(test_id, m, a_arr); test_id += 1

    # --- БЛОК 3: Тесты на погрешность float/double и переполнение (20 тестов: 021-040) ---
    # Значения подобраны так, чтобы отношение секунд к стоимости было очень близким
    # Например: 999999999 / 2^10 и 1000000000 / 2^10. Float на этом гарантированно упадет.
    precision_a1 = [MAX_A - i for i in range(31)]
    write_test(test_id, 5 * 10**8, precision_a1); test_id += 1
    
    precision_a2 = [MAX_A if i % 2 == 0 else MAX_A - 1 for i in range(31)]
    write_test(test_id, MAX_M, precision_a2); test_id += 1

    # Ловушки, где выгоднее купить одну дорогую карту с избытком, чем много дешевых тютелька в тютельку
    trap_a = [1] * 29 + [10, 10**8]  # Предпоследняя дает 10 сек за 2^29, последняя дает 10^8 сек за 2^30
    write_test(test_id, 100, trap_a); test_id += 1

    for _ in range(17):
        m = random.randint(10**5, 10**7)
        # Близкие значения около одной константы
        base = random.randint(10**6, 10**7)
        a_arr = [base + random.randint(-10, 10) for _ in range(31)]
        write_test(test_id, m, a_arr); test_id += 1

    # --- БЛОК 4: Средние случайные тесты (40 тестов: 041-080) ---
    for _ in range(40):
        m = random.randint(10**4, 10**8)
        a_arr = [random.randint(1, 10**6) for _ in range(31)]
        write_test(test_id, m, a_arr); test_id += 1

    # --- БЛОК 5: Большие стресс-тесты на максимальных ограничениях (40 тестов: 081-120) ---
    # 081-090: m вблизи 10^9, карточки дают очень мало секунд (Ответ будет огромным, тест на long long)
    for _ in range(10):
        m = random.randint(MAX_M - 1000, MAX_M)
        a_arr = [random.randint(1, 10) for _ in range(31)]
        write_test(test_id, m, a_arr); test_id += 1

    # 091-100: Все a_i равны 1 (По сути, оптимально брать самую дешевую карту за 1 рубль)
    for _ in range(10):
        m = random.randint(10**8, MAX_M)
        write_test(test_id, m, [1] * 31); test_id += 1

    # 101-110: Абсолютно хаотичный шум по всему доступному диапазону натуральных чисел
    for _ in range(10):
        m = random.randint(1, MAX_M)
        a_arr = [random.randint(1, MAX_A) for _ in range(31)]
        write_test(test_id, m, a_arr); test_id += 1

    # 111-120: Карточки с чередующейся эффективностью (одни супер-выгодные, другие пустышки)
    for _ in range(10):
        m = random.randint(10**7, MAX_M)
        a_arr = []
        for i in range(31):
            if i % 3 == 0:
                a_arr.append(random.randint(10**7, MAX_A))
            else:
                a_arr.append(random.randint(1, 100))
        write_test(test_id, m, a_arr); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, m, a_arr):
    """Вспомогательная функция для записи теста в файл формата XXX.in с трехзначной нумерацией"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{m}\n")
        f.write(" ".join(map(str, a_arr)) + "\n")

if __name__ == "__main__":
    random.seed(303030)
    generate_tests()