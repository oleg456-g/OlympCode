#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_N = 1000
    MAX_K = 500
    MAX_TIME = 100

    # --- БЛОК 1: Примеры из условия (2 теста: 001-002) ---
    write_test(test_id, 10, 3, [6, 2, 8]); test_id += 1
    write_test(test_id, 3, 2, [10, 20]); test_id += 1

    # --- БЛОК 2: Краевые кейсы и минимальные ограничения (15 тестов: 003-017) ---
    write_test(test_id, 1, 1, [1]); test_id += 1                     # Минимальный тест, ровно хватает времени
    write_test(test_id, 1, 1, [2]); test_id += 1                     # Времени меньше, чем нужно на 1 сапог
    write_test(test_id, 10, 1, [5]); test_id += 1                    # Времени сильно больше, чем нужно
    write_test(test_id, 100, 5, [20, 20, 20, 20, 20]); test_id += 1  # Время заканчивается ровно "в ноль"
    
    # Все сапоги можно починить (Ловушка для решений без проверки выхода за границы вектора)
    write_test(test_id, 100, 3, [1, 2, 3]); test_id += 1
    write_test(test_id, 1000, 5, [10, 10, 10, 10, 10]); test_id += 1
    
    # Все сапоги требуют одинакового времени починки
    write_test(test_id, 50, 10, [10] * 10); test_id += 1
    write_test(test_id, 45, 10, [10] * 10); test_id += 1            # Не хватит на последний
    
    # Сапоги с максимальной стоимостью починки (100 минут)
    write_test(test_id, 100, 3, [100, 100, 100]); test_id += 1
    write_test(test_id, 99, 3, [100, 100, 100]); test_id += 1
    
    # Один очень дешевый сапог, остальные — неподъемные
    write_test(test_id, 5, 5, [5, 100, 100, 100, 100]); test_id += 1
    write_test(test_id, 4, 5, [5, 100, 100, 100, 100]); test_id += 1
    
    # Сапоги идут строго по возрастанию / убыванию изначально
    write_test(test_id, 15, 5, [1, 2, 3, 4, 5]); test_id += 1
    write_test(test_id, 15, 5, [5, 4, 3, 2, 1]); test_id += 1
    write_test(test_id, 10, 4, [4, 3, 2, 1]); test_id += 1

    # --- БЛОК 3: Тесты против неправильной сортировки (по убыванию или без неё) (23 теста: 018-040) ---
    # Если брать большие элементы, ответ будет 1 или 0, если мелкие — много
    write_test(test_id, 100, 5, [99, 1, 1, 1, 1]); test_id += 1
    write_test(test_id, 50, 6, [45, 10, 10, 10, 10, 10]); test_id += 1
    
    # "Анти-жадный" выбор по убыванию
    write_test(test_id, 30, 4, [25, 10, 10, 10]); test_id += 1
    write_test(test_id, 100, 10, [90] + [11]*9); test_id += 1
    
    # Перемешанные ручные каверзные наборы
    for _ in range(19):
        # Структура: один гигантский элемент в случайном месте, много мелких
        boots = [random.randint(1, 5) for _ in range(10)] + [95]
        random.shuffle(boots)
        write_test(test_id, 40, len(boots), boots); test_id += 1

    # --- БЛОК 4: Средние случайные тесты (40 тестов: 041-080) ---
    for _ in range(40):
        n = random.randint(50, 500)
        k = random.randint(10, 100)
        boots = [random.randint(1, MAX_TIME) for _ in range(k)]
        write_test(test_id, n, k, boots); test_id += 1

    # --- БЛОК 5: Максимальные стресс-тесты (40 тестов: 081-120) ---
    # N = 1000, K = 500 — проверка работы на верхних границах

    # 081-090: Все сапоги по 1 минуте (самый длинный цикл прохода по массиву)
    for _ in range(10):
        n = random.randint(1, MAX_N)
        boots = [1] * MAX_K
        write_test(test_id, n, MAX_K, boots); test_id += 1

    # 091-100: Все сапоги по 100 минут (максимальная стоимость)
    for _ in range(10):
        n = random.randint(1, MAX_N)
        boots = [100] * MAX_K
        write_test(test_id, n, MAX_K, boots); test_id += 1

    # 101-110: Абсолютно случайный шум на максимальных ограничениях
    for _ in range(10):
        boots = [random.randint(1, MAX_TIME) for _ in range(MAX_K)]
        write_test(test_id, MAX_N, MAX_K, boots); test_id += 1

    # 111-120: Плотные группы близких по значению элементов
    for _ in range(10):
        base = random.randint(1, 80)
        boots = [random.randint(base, base + 20) for _ in range(MAX_K)]
        n = random.randint(300, MAX_N)
        write_test(test_id, n, MAX_K, boots); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, k, boots):
    """Вспомогательная функция для записи теста в файл формата XXX.in с трехзначной нумерацией"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n} {k}\n")
        f.write(" ".join(map(str, boots)) + "\n")

if __name__ == "__main__":
    random.seed(909090)
    generate_tests()