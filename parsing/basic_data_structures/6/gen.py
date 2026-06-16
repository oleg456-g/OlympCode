#!/usr/bin/env python3
import random
import os

def generate_tests():
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1

    # --- БЛОК 1: Пример из условия (1) ---
    write_test(test_id, [
        "size", "push 1", "size", "push 2", "size", "push 3", "size", "exit"
    ]); test_id += 1

    # --- БЛОК 2: Тесты на базовые одиночные операции и пустую очередь (2-15) ---
    write_test(test_id, ["size", "exit"]); test_id += 1
    write_test(test_id, ["clear", "size", "exit"]); test_id += 1
    write_test(test_id, ["push 5", "front", "exit"]); test_id += 1
    write_test(test_id, ["push -10", "pop", "exit"]); test_id += 1
    write_test(test_id, ["push 42", "clear", "size", "exit"]); test_id += 1
    
    # Несколько элементов подряд
    write_test(test_id, ["push 100", "push 200", "front", "pop", "front", "pop", "exit"]); test_id += 1
    write_test(test_id, ["push 1", "push 2", "size", "clear", "size", "exit"]); test_id += 1
    
    # Проверка на то, что `front` не удаляет элемент, а `pop` удаляет
    write_test(test_id, ["push 7", "front", "front", "size", "pop", "size", "exit"]); test_id += 1
    
    # Различные значения n (отрицательные, большие)
    write_test(test_id, ["push -99999", "pop", "exit"]); test_id += 1
    write_test(test_id, ["push 0", "pop", "exit"]); test_id += 1
    write_test(test_id, ["push 123456789", "front", "exit"]); test_id += 1
    
    # Чередование push и pop
    write_test(test_id, ["push 1", "pop", "push 2", "pop", "push 3", "pop", "size", "exit"]); test_id += 1
    write_test(test_id, ["push 5", "push 6", "pop", "push 7", "pop", "pop", "exit"]); test_id += 1

    # --- БЛОК 3: Серии заполнения до максимального гарантированного размера 100 (16-45) ---
    # Ловит ошибки статических массивов неверного размера
    for cap in range(70, 100):
        commands = []
        for i in range(cap):
            commands.append(f"push {i}")
        commands.append("size")
        commands.append("front")
        for _ in range(cap):
            commands.append("pop")
        commands.append("size")
        commands.append("exit")
        write_test(test_id, commands); test_id += 1

    # --- БЛОК 4: Тесты с частым использованием команды clear (46-75) ---
    # Проверяет сброс указателей head и tail в пользовательской реализации
    for run in range(30):
        commands = []
        for i in range(10):
            commands.append(f"push {i * run}")
        commands.append("clear")
        commands.append("size")
        for i in range(5):
            commands.append(f"push {i}")
        commands.append("front")
        for _ in range(5):
            commands.append("pop")
        commands.append("exit")
        write_test(test_id, commands); test_id += 1

    # --- БЛОК 5: Случайные сценарии средней длины (76-100) ---
    for _ in range(25):
        commands = []
        in_queue = 0
        current_val = 1
        for _ in range(200):
            # Поддерживаем размер строго <= 100
            if in_queue == 0:
                op = "push"
            elif in_queue >= 90:
                op = random.choice(["pop", "front", "size", "clear"])
            else:
                op = random.choice(["push", "pop", "front", "size", "clear"])
                
            if op == "push":
                commands.append(f"push {current_val}")
                current_val += 1
                in_queue += 1
            elif op == "pop":
                commands.append("pop")
                in_queue -= 1
            elif op == "front":
                commands.append("front")
            elif op == "size":
                commands.append("size")
            elif op == "clear":
                commands.append("clear")
                in_queue = 0
        commands.append("exit")
        write_test(test_id, commands); test_id += 1

    # --- БЛОК 6: Большие стресс-тесты (101-120) ---
    # До 10 000 команд в тесте. Очередь "ползет" по памяти вперед-назад. 
    # Если использовать простой массив без сдвига или кольцевого буфера, память/индексы переполнятся.
    for _ in range(20):
        commands = []
        in_queue = 0
        current_val = 1
        # Суммарно много операций, но в очереди одновременно мало элементов
        for _ in range(5000):
            if in_queue == 0:
                commands.append(f"push {current_val}")
                current_val += 1
                in_queue += 1
            else:
                choice = random.random()
                if choice < 0.45:
                    if in_queue < 95:
                        commands.append(f"push {current_val}")
                        current_val += 1
                        in_queue += 1
                elif choice < 0.90:
                    commands.append("pop")
                    in_queue -= 1
                elif choice < 0.95:
                    commands.append("front")
                else:
                    commands.append("size")
        commands.append("clear")
        commands.append("exit")
        write_test(test_id, commands); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")


def write_test(test_id, commands):
    """Вспомогательная функция для записи файла теста"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        for cmd in commands:
            f.write(cmd + "\n")


if __name__ == "__main__":
    random.seed(111222)
    generate_tests()