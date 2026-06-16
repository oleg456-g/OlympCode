#!/usr/bin/env python3
import random
import os

def generate_tests():
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1

    # --- БЛОК 1: Пример из условия (1) ---
    write_test(test_id, "8 9 + 1 7 - *"); test_id += 1

    # --- БЛОК 2: Минимальные и краевые кейсы (2-15) ---
    write_test(test_id, "42"); test_id += 1                           # Одно положительное число
    write_test(test_id, "-15"); test_id += 1                          # Одно отрицательное число
    write_test(test_id, "0"); test_id += 1                            # Ноль
    write_test(test_id, "10 20 +"); test_id += 1                      # Простое сложение
    write_test(test_id, "5 12 -"); test_id += 1                       # Вычитание с отрицательным результатом
    write_test(test_id, "12 5 -"); test_id += 1                       # Вычитание с положительным результатом
    write_test(test_id, "0 10 -"); test_id += 1                       # Вычитание из нуля
    write_test(test_id, "7 -3 *"); test_id += 1                       # Умножение на отрицательное число
    write_test(test_id, "-4 -5 *"); test_id += 1                      # Умножение двух отрицательных чисел
    write_test(test_id, "100 100 *"); test_id += 1                    # Граничные входные значения 100 по модулю
    write_test(test_id, "-100 100 +"); test_id += 1                   # Граничные входные значения
    
    # Тесты с избыточными пробелами
    write_test(test_id, "  5   6   +  "); test_id += 1 
    write_test(test_id, "10 5 -      "); test_id += 1
    write_test(test_id, "3"); test_id += 1

    # --- БЛОК 3: Тесты на порядок операндов при вычитании (16-45) ---
    for i in range(15):
        a = random.randint(-100, 100)
        b = random.randint(-100, 100)
        write_test(test_id, f"{a} {b} -"); test_id += 1
        write_test(test_id, f"{b} {a} -"); test_id += 1

    # --- БЛОК 4: Комбинированные выражения средней длины (46-75) ---
    for _ in range(30):
        # Ограничиваем глубину, контролируя значения
        expr_str, _ = generate_safe_postfix(max_depth=4)
        tokens = expr_str.split()
        spaced_expr = ""
        for token in tokens:
            spaced_expr += token + " " * random.randint(1, 4)
        write_test(test_id, spaced_expr); test_id += 1

    # --- БЛОК 5: Глубокие вложенные структуры (76-100) ---
    # Линейные цепочки ("числа сначала, операции потом"), строго контролирующие рост значений в int
    for _ in range(25):
        length = random.randint(10, 20)
        nums = []
        ops = []
        current_val = random.randint(-10, 10)
        nums.append(str(current_val))
        
        for _ in range(length - 1):
            next_num = random.randint(-10, 10)
            op = random.choice(["+", "-", "*"])
            
            # Гарантируем, что симуляция операции не выйдет за безопасные пределы знакового int
            # Считаем "в уме" в порядке постфиксного разбора линейного стека
            if op == "+":
                new_val = current_val + next_num
            elif op == "-":
                new_val = current_val - next_num
            else:
                new_val = current_val * next_num
                
            # Если вышли за рамки 10^8, принудительно откатываемся на безопасное сложение/вычитание нуля
            if abs(new_val) > 10**8:
                op = "+"
                next_num = 0
            
            nums.append(str(next_num))
            ops.append(op)
            current_val = current_val + next_num if op == "+" else (current_val - next_num if op == "-" else current_val * next_num)
            
        expr = " ".join(nums) + " " + " ".join(ops)
        write_test(test_id, expr); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты без переполнения (101-120) ---
    # Длинные выражения (большое число токенов), но с жестко зажатыми промежуточными результатами
    for _ in range(20):
        expr_str, _ = generate_safe_postfix(max_depth=7)
        write_test(test_id, expr_str); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")


def generate_safe_postfix(max_depth, current_depth=0):
    """
    Генератор постфиксных выражений, который на каждом шагу проверяет 
    реальный математический результат и не позволяет выйти за границы безопасного int.
    """
    # Базовый случай: лист дерева (просто число от -100 до 100 по условию)
    if current_depth >= max_depth or (current_depth > 1 and random.random() < 0.3):
        val = random.randint(-100, 100)
        return str(val), val
    
    left_str, left_val = generate_safe_postfix(max_depth, current_depth + 1)
    right_str, right_val = generate_safe_postfix(max_depth, current_depth + 1)
    
    op = random.choice(["+", "-", "*"])
    
    # Защитный лимит промежуточных результатов (в пределах 1.5 * 10^9, что абсолютно безопасно для int)
    SAFE_INT_LIMIT = 15 * 10**8
    
    if op == "+":
        res = left_val + right_val
    elif op == "-":
        res = left_val - right_val
    else:
        res = left_val * right_val
        
    # Если зафиксировано потенциальное переполнение типа int, переключаем операцию на безопасное сложение
    if abs(res) > SAFE_INT_LIMIT:
        op = "+"
        res = left_val + right_val
        # Если даже сложение вылетает (маловероятно при базовых числах, но всё же)
        if abs(res) > SAFE_INT_LIMIT:
            op = "-"
            res = left_val - right_val
            if abs(res) > SAFE_INT_LIMIT:
                # В крайнем случае зануляем правый операнд в строке и результате
                right_str = "0"
                op = "+"
                res = left_val
                
    return f"{left_str} {right_str} {op}", res


def write_test(test_id, expr):
    """Вспомогательная функция для записи файла теста"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(expr + "\n")


if __name__ == "__main__":
    random.seed(987123)
    generate_tests()