#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MIN_VAL, MAX_VAL = -1000, 1000

    # --- БЛОК 1: Примеры из условия (1) ---
    write_test(test_id, 1, -3, 3, -1); test_id += 1 # Корень x = 1.0

    # --- БЛОК 2: Маленькие ручные и вырожденные случаи (2-12) ---
    # Точные целые корни без квадратного остатка (p=0, q=0 -> три кратных корня, что тоже считается за ОДИН уникальный корень)
    write_test(test_id, 1, 0, 0, 0); test_id += 1       # x^3 = 0 -> x = 0
    write_test(test_id, -1, 0, 0, 0); test_id += 1      # -x^3 = 0 -> x = 0
    write_test(test_id, 1, 0, 0, -1); test_id += 1      # x^3 - 1 = 0 -> x = 1
    write_test(test_id, 1, 0, 0, 8); test_id += 1       # x^3 + 8 = 0 -> x = -2
    write_test(test_id, 1, -3, 3, -1); test_id += 1     # (x-1)^3 = 0 -> x = 1
    write_test(test_id, 1, 3, 3, 1); test_id += 1       # (x+1)^3 = 0 -> x = -1
    
    # Случаи, когда корень x = 0, а оставшийся квадратный трехчлен не имеет корней (q > 0, p^2 - 4q < 0)
    # a=1, x0=0, p=0, q=1 -> x*(x^2 + 1) = x^3 + x = 0
    write_test(test_id, 1, 0, 1, 0); test_id += 1
    # a=5, x0=0, p=2, q=10 -> 5x^3 + 10x^2 + 50x = 0
    write_test(test_id, 5, 10, 50, 0); test_id += 1
    # Коэффициенты на границах диапазона для простых кубов
    write_test(test_id, 1, 0, 0, -1000); test_id += 1   # x = 10
    write_test(test_id, 1, 0, 0, 1000); test_id += 1    # x = -10
    write_test(test_id, 1000, 0, 0, 0); test_id += 1    # 1000x^3 = 0 -> x = 0

    # Вспомогательная функция генерации валидных коэффициентов через разложение
    def get_valid_cubic():
        while True:
            a = random.choice([i for i in range(-50, 51) if i != 0])
            # Дробные или целые потенциальные корни x0
            x0 = random.uniform(-20, 20)
            # Подбираем p и q так, чтобы дискриминант p^2 - 4q был отрицательным
            p = random.uniform(-10, 10)
            q = random.uniform(p**2 / 4.0 + 0.5, p**2 / 4.0 + 50.0)
            
            # Вычисляем теоретические коэффициенты
            b_val = a * (p - x0)
            c_val = a * (q - p * x0)
            d_val = -a * q * x0
            
            # Округляем до целых, так как по условию коэффициенты целые
            b, c, d = int(round(b_val)), int(round(c_val)), int(round(d_val))
            
            # Строгая проверка: попали ли мы в лимиты задачи
            if MIN_VAL <= b <= MAX_VAL and MIN_VAL <= c <= MAX_VAL and MIN_VAL <= d <= MAX_VAL:
                # Финальная математическая проверка дискриминанта кубического уравнения
                # Чтобы гарантировать ровно 1 корень (без пограничных зацеплений из-за округления)
                delta = 18*a*b*c*d - 4*b**3*d + b**2*c**2 - 4*a*c**3 - 27*a**2*d**2
                if delta < 0:
                    return a, b, c, d

    # --- БЛОК 3: Тесты с экстремальными значениями коэффициентов (13-25) ---
    # Ищем уравнения, где хотя бы один из параметров b, c или d близок к максимуму/минимуму (+-1000)
    while test_id <= 25:
        a, b, c, d = get_valid_cubic()
        if any(abs(x) > 850 for x in [b, c, d]):
            write_test(test_id, a, b, c, d)
            test_id += 1

    # --- БЛОК 4: Тесты с очень маленькими коэффициентами (близко к 0, но а != 0) (26-35) ---
    while test_id <= 35:
        a, b, c, d = get_valid_cubic()
        if all(abs(x) < 50 for x in [b, c, d]):
            write_test(test_id, a, b, c, d)
            test_id += 1

    # --- БЛОК 5: Асимметричные тесты (разные знаки коэффициентов) (36-48) ---
    # Важно для бинарного поиска, чтобы функция не всегда была "идеально симметричной" параболой
    while test_id <= 48:
        a, b, c, d = get_valid_cubic()
        # Ищем тесты, где знаки чередуются или имеют сложную структуру (например, ++--)
        signs = [x > 0 for x in [a, b, c, d]]
        if signs.count(True) == 2: # Ровно два положительных и два отрицательных коэффициента
            write_test(test_id, a, b, c, d)
            test_id += 1

    # --- БЛОК 6: Большие случайные математически строгие тесты (49-60) ---
    while test_id <= 60:
        a, b, c, d = get_valid_cubic()
        write_test(test_id, a, b, c, d)
        test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, a, b, c, d):
    """Вспомогательная функция для записи теста в файл формата XX.in"""
    filename = f"tests/{str(test_id).zfill(2)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{a} {b} {c} {d}\n")

if __name__ == "__main__":
    # Фиксируем seed для полной детерминированности пакета тестов
    random.seed(555777)
    generate_tests()