#!/usr/bin/env python3
import random
import os

def generate_tests():
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1

    # --- БЛОК 1: Примеры из условия (1-2) ---
    write_test(test_id, "()[]"); test_id += 1
    write_test(test_id, "([)]"); test_id += 1

    # --- БЛОК 2: Краевые и минимальные тесты (3-15) ---
    write_test(test_id, ""); test_id += 1              # Пустая строка -> yes
    write_test(test_id, "()"); test_id += 1            # Круглые
    write_test(test_id, "[]"); test_id += 1            # Квадратные
    write_test(test_id, "{}"); test_id += 1            # Фигурные
    write_test(test_id, "("); test_id += 1             # Только одна открывающая -> no
    write_test(test_id, "]"); test_id += 1             # Только одна закрывающая -> no
    write_test(test_id, "}{"); test_id += 1            # Неверный порядок -> no
    write_test(test_id, "(]"); test_id += 1            # Разные типы -> no
    
    # Прямая ловушка для некорректного кода (вложенность без смежности в строке)
    write_test(test_id, "[()]"); test_id += 1          # Корректная, но ломает проверку a[i-1] -> yes
    write_test(test_id, "{[()]}"); test_id += 1        # Глубокая вложенность -> yes
    write_test(test_id, "({[]})"); test_id += 1        # Еще один вариант -> yes
    write_test(test_id, "(((())))"); test_id += 1      # Однотипная глубокая вложенность -> yes

    # --- БЛОК 3: Тесты средней длины на вложенность (16-45) ---
    # Генерируем сбалансированные и разбалансированные последовательности с вложенной структурой
    for size in range(5, 35):
        # Делаем правильную последовательность вида: [ [ [ ... ] ] ] + ( )
        seq = "[" * size + "()" + "]" * size
        if size % 2 == 0:
            # Делаем её ошибочной, заменив одну скобку в самом конце
            seq = seq[:-1] + ")"
        write_test(test_id, seq); test_id += 1

    # --- БЛОК 4: Последовательные структуры и миксы (46-75) ---
    # Много правильных пар подряд вида ()()()[]{}[]
    for _ in range(15):
        pairs = ["()", "[]", "{}"]
        seq = "".join(random.choices(pairs, k=20))
        write_test(test_id, seq); test_id += 1

    # Почти правильные последовательные структуры с ошибкой в середине или конце
    for _ in range(15):
        pairs = ["()", "[]", "{}"]
        seq = "".join(random.choices(pairs, k=20))
        # Ломаем структуру, вставив лишнюю закрывающую или удалив открывающую
        pos = random.randint(0, len(seq))
        seq = seq[:pos] + random.choice([")", "]", "}"]) + seq[pos:]
        write_test(test_id, seq); test_id += 1

    # --- БЛОК 5: Случайные комбинированные тесты (76-100) ---
    for _ in range(25):
        # Генерируем случайную правильную последовательность через генерацию дерева
        seq = generate_valid_brackets(random.randint(50, 250))
        if random.nextfloat() < 0.3 if hasattr(random, 'nextfloat') else random.random() < 0.3:
            # С вероятностью 30% делаем её неправильной путем случайной мутации символа
            idx = random.randint(0, len(seq) - 1)
            seq = seq[:idx] + random.choice(["(", "]", "{", ")"]) + seq[idx+1:]
        write_test(test_id, seq); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (101-120) ---
    # Длина строк приближается или равна 100 000 символов
    max_len = 100000

    # 101-105: Огромные правильные последовательности (вложенные + последовательные)
    for _ in range(5):
        seq = generate_valid_brackets(max_len // 2)
        write_test(test_id, seq); test_id += 1

    # 106-110: Огромные последовательности, состоящие только из открывающих скобок
    for _ in range(5):
        seq = "".join(random.choices(["(", "[", "{"], k=max_len))
        write_test(test_id, seq); test_id += 1

    # 111-115: Огромные последовательности, состоящие только из закрывающих скобок
    for _ in range(5):
        seq = "".join(random.choices([")", "]", "}"], k=max_len))
        write_test(test_id, seq); test_id += 1

    # 116-120: Огромная правильная последовательность, испорченная ровно в одном (последнем) символе
    for _ in range(5):
        seq = generate_valid_brackets(max_len // 2)
        # Заменяем самую последнюю закрывающую скобку на неверный тип
        seq = seq[:-1] + (")" if seq[-1] != ")" else "]")
        write_test(test_id, seq); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")


def generate_valid_brackets(num_pairs):
    """Вспомогательная функция для генерации случайной ПСП"""
    brackets = [("(", ")"), ("[", "]"), ("{", "}")]
    
    # Классический алгоритм генерации случайной ПСП через обход дерева / случайные вставки
    # Для высокой скорости на больших длинах соберем блоками
    tokens = []
    stack = []
    
    for _ in range(num_pairs):
        if not stack or random.random() < 0.5:
            b_open, b_close = random.choice(brackets)
            tokens.append(b_open)
            stack.append(b_close)
        else:
            tokens.append(stack.pop())
            
    while stack:
        tokens.append(stack.pop())
        
    return "".join(tokens)


def write_test(test_id, seq):
    """Вспомогательная функция для записи теста"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(seq + "\n")


if __name__ == "__main__":
    random.seed(123456)
    generate_tests()