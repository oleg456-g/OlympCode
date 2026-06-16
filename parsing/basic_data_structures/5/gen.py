#!/usr/bin/env python3
import random
import os

def generate_tests():
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    MAX_N = 100000

    # --- БЛОК 1: Пример из условия (1) ---
    write_test(test_id, [
        "+ 1", "+ 2", "-", "+ 3", "* 4", "-", "-"
    ]); test_id += 1

    # --- БЛОК 2: Минимальные и краевые кейсы (2-12) ---
    write_test(test_id, ["+ 10", "-"]); test_id += 1                 # 1 гоблин пришел и ушел
    write_test(test_id, ["* 5", "-"]); test_id += 1                  # 1 привилегированный пришел и ушел
    write_test(test_id, ["+ 1", "+ 2", "-", "-"]); test_id += 1      # Два обычных
    write_test(test_id, ["+ 1", "* 2", "-", "-"]); test_id += 1      # Обычный, затем привилегированный
    write_test(test_id, ["* 1", "+ 2", "-", "-"]); test_id += 1      # Привилегированный, затем обычный
    write_test(test_id, ["* 1", "* 2", "-", "-"]); test_id += 1      # Два привилегированных
    
    # Разные сценарии для 3 элементов (проверка округления середины при нечетной длине)
    write_test(test_id, ["+ 1", "+ 2", "+ 3", "-", "-", "-"]); test_id += 1
    write_test(test_id, ["+ 1", "+ 2", "* 3", "-", "-", "-"]); test_id += 1
    write_test(test_id, ["+ 1", "* 2", "+ 3", "-", "-", "-"]); test_id += 1
    write_test(test_id, ["+ 1", "* 2", "* 3", "-", "-", "-"]); test_id += 1
    write_test(test_id, ["* 1", "* 2", "* 3", "-", "-", "-"]); test_id += 1

    # --- БЛОК 3: Длинные серии однотипных операций (13-42) ---
    # Ровно 30 тестов. Проверяют стабильность структуры при накоплении
    for size in range(15, 45):
        queries = []
        # Накапливаем гоблинов
        for i in range(1, size + 1):
            if size % 2 == 0:
                queries.append(f"+ {i}")
            else:
                queries.append(f"* {i}")
        # Полностью освобождаем очередь
        for _ in range(size):
            queries.append("-")
        write_test(test_id, queries); test_id += 1

    # --- БЛОК 4: Чередующиеся циклы "пришел / ушел" (43-72) ---
    # Ровно 30 тестов. Ломают логику, если балансировка работает только на вставку
    for t in range(30):
        queries = []
        current_id = 1
        # Очередь пульсирует вокруг размера 2-5 элементов
        for i in range(40):
            if i % 3 == 0:
                queries.append(f"+ {current_id}")
                current_id += 1
            elif i % 3 == 1:
                queries.append(f"* {current_id}")
                current_id += 1
            else:
                if current_id > 2: # Гарантируем непустоту
                    queries.append("-")
        # Очистка остатков
        while queries.count("+") + queries.count("*") > queries.count("-"):
            queries.append("-")
        write_test(test_id, queries); test_id += 1

    # --- БЛОК 5: Случайные тесты среднего размера (73-100) ---
    # Ровно 28 тестов для точного баланса до 100
    for _ in range(28):
        n_queries = random.randint(500, 1500)
        queries = []
        current_id = 1
        in_queue = 0
        for _ in range(n_queries):
            op = random.choice(["+", "*", "-"])
            if op == "-" and in_queue > 0:
                queries.append("-")
                in_queue -= 1
            else:
                op_type = random.choice(["+", "*"])
                queries.append(f"{op_type} {current_id}")
                current_id += 1
                in_queue += 1
        # Гарантированная очистка всех, кто остался
        for _ in range(in_queue):
            queries.append("-")
        write_test(test_id, queries); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (101-120) ---
    # Ровно 20 тестов с N = 100 000. Жесткая проверка на O(1) каждой операции.
    
    # 101-105: Только обычные гоблины (Классическая очередь)
    for _ in range(5):
        queries = []
        for i in range(1, MAX_N // 2 + 1):
            queries.append(f"+ {i}")
        for _ in range(MAX_N // 2):
            queries.append("-")
        write_test(test_id, queries); test_id += 1

    # 106-110: Только привилегированные гоблины 
    for _ in range(5):
        queries = []
        for i in range(1, MAX_N // 2 + 1):
            queries.append(f"* {i}")
        for _ in range(MAX_N // 2):
            queries.append("-")
        write_test(test_id, queries); test_id += 1

    # 111-115: Массовый наплыв гоблинов, затем массовый уход к шаманам (Микс типов)
    for _ in range(5):
        queries = []
        for i in range(1, MAX_N // 2 + 1):
            op_type = "+" if random.random() < 0.6 else "*"
            queries.append(f"{op_type} {i}")
        for _ in range(MAX_N // 2):
            queries.append("-")
        write_test(test_id, queries); test_id += 1

    # 116-120: Полностью случайные запросы максимальной длины
    for _ in range(5):
        queries = []
        current_id = 1
        in_queue = 0
        for _ in range(MAX_N - 1000):
            if in_queue > 0 and random.random() < 0.4:
                queries.append("-")
                in_queue -= 1
            else:
                op_type = "+" if random.random() < 0.7 else "*"
                queries.append(f"{op_type} {current_id}")
                current_id += 1
                in_queue += 1
        for _ in range(in_queue):
            queries.append("-")
        write_test(test_id, queries); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")


def write_test(test_id, queries):
    """Вспомогательная функция для записи тест-кейса в файл"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{len(queries)}\n")
        for q in queries:
            f.write(q + "\n")


if __name__ == "__main__":
    random.seed(333222)
    generate_tests()