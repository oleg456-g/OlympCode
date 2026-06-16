#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_VAL = 10**18

    # --- БЛОК 1: Примеры из условия (1-2) ---
    write_test(test_id, 11, 2, 3, 21, 25); test_id += 1
    write_test(test_id, 1, 5, 5, 6, 6); test_id += 1

    # --- БЛОК 2: Минимальные и краевые кейсы (3-15) ---
    write_test(test_id, 1, 1, 1, 1, 1); test_id += 1               # Всё по 1 (Ответ: 0)
    write_test(test_id, 1, 1, 1, 3, 3); test_id += 1               # Ровно под d = 1 (1+2x1 = 3 -> Ответ: 1)
    write_test(test_id, 2, 1, 1, 3, 3); test_id += 1               # Места под 2 модуля с d=1 уже нет (Ответ: 0)
    write_test(test_id, 1, 10, 10, 11, 11); test_id += 1           # Чуть-чуть не хватает до d = 1 (Ответ: 0)
    write_test(test_id, 4, 2, 2, 8, 8); test_id += 1               # Идеальная квадратная сетка 2х2
    
    # Тесты, где выгодно развернуть модули на 90 градусов
    write_test(test_id, 2, 1, 10, 11, 10); test_id += 1            # Без поворота не влезет (Ответ: 0)
    write_test(test_id, 1, 1, 5, 10, 3); test_id += 1              # Поворот даёт d = 1 

    # Большое количество модулей, маленькие размеры
    write_test(test_id, 1000, 1, 1, 100, 10); test_id += 1         # Заполнение в линию
    write_test(test_id, 1000, 1, 1, 10, 100); test_id += 1         # Заполнение в линию по вертикали
    write_test(test_id, 10**6, 1, 1, 1000, 1000); test_id += 1     # Ровно миллион модулей 1х1 в поле 1000х1000 (Ответ: 0)

    # --- БЛОК 3: Тесты средней величины без переполнения (16-25) ---
    for _ in range(5):
        # Обычные случайные тесты со значениями до 10^6
        a, b = random.randint(1, 100), random.randint(1, 100)
        d = random.randint(1, 50)
        cols, rows = random.randint(1, 20), random.randint(1, 20)
        n = cols * rows
        w = cols * (a + 2*d) + random.randint(0, 10)
        h = rows * (b + 2*d) + random.randint(0, 10)
        write_test(test_id, n, a, b, w, h); test_id += 1

    for _ in range(5):
        # Тесты со случайными параметрами среднего размера
        n = random.randint(5, 100)
        a = random.randint(10, 1000)
        b = random.randint(10, 1000)
        w = random.randint(2000, 50000)
        h = random.randint(2000, 50000)
        write_test(test_id, n, a, b, w, h); test_id += 1

    # --- БЛОК 4: Жесткие тесты на переполнение типов (Потенциальный Overflow) (26-40) ---
    # Размеры поля огромные (~10^18), а размеры модулей маленькие. 
    # При попытке умножить (w / (a+2d)) * (h / (b+2d)) в коде C++ произойдет переполнение long long, 
    # если не делать проверку делением или через __int128.
    for _ in range(5):
        write_test(test_id, 1, 1, 1, MAX_VAL, MAX_VAL); test_id += 1
        
    for _ in range(5):
        n = random.randint(10**14, 10**16)
        a, b = random.randint(1, 5), random.randint(1, 5)
        w = random.randint(10**17, MAX_VAL)
        h = random.randint(10**17, MAX_VAL)
        write_test(test_id, n, a, b, w, h); test_id += 1

    for _ in range(5):
        # N маленькое, но стороны поля и ответ гигантские
        n = random.randint(1, 10)
        a, b = random.randint(10**9, 10**12), random.randint(10**9, 10**12)
        w = random.randint(10**17, MAX_VAL)
        h = random.randint(10**17, MAX_VAL)
        write_test(test_id, n, a, b, w, h); test_id += 1

    # --- БЛОК 5: Асимметричные гигантские тесты (41-50) ---
    # Поля вытянуты в длинную и узкую полосу вдоль одной из осей
    for _ in range(5):
        w = random.randint(10**15, MAX_VAL)
        h = random.randint(1, 100)
        a, b = random.randint(1, 10), random.randint(1, 10)
        n = random.randint(1, 10**9)
        write_test(test_id, n, a, b, w, h); test_id += 1
        
    for _ in range(5):
        w = random.randint(1, 100)
        h = random.randint(10**15, MAX_VAL)
        a, b = random.randint(1, 10), random.randint(1, 10)
        n = random.randint(1, 10**9)
        write_test(test_id, n, a, b, w, h); test_id += 1

    # --- БЛОК 6: Абсолютный стресс-тест по всем фронтам (51-60) ---
    # Все параметры выбираются случайно в диапазоне до 10^18. 
    # Проверяет корректность выбора правой границы бинарного поиска (R = max(w, h)).
    for _ in range(10):
        n = random.randint(1, MAX_VAL)
        a = random.randint(1, MAX_VAL)
        b = random.randint(1, MAX_VAL)
        w = random.randint(1, MAX_VAL)
        h = random.randint(1, MAX_VAL)
        
        # Чтобы тест гарантированно удовлетворял условию "при d=0 решение существует", 
        # принудительно расширим w и h под случайную сетку
        cols = random.randint(1, 100)
        rows = (n + cols - 1) // cols
        w = max(w, cols * a)
        h = max(h, rows * b)
        
        write_test(test_id, n, a, b, w, h); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, a, b, w, h):
    """Вспомогательная функция для записи теста в файл формата XX.in"""
    filename = f"tests/{str(test_id).zfill(2)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n} {a} {b} {w} {h}\n")

if __name__ == "__main__":
    # Фиксируем seed для полной воспроизводимости пакета тестов
    random.seed(11111111)
    generate_tests()