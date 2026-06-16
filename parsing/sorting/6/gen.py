#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    alphabet = "abcdefghijklmnopqrstuvwxyz" # Только строчные латинские буквы
    max_len = 100000

    # --- БЛОК 1: Примеры из условия (1-2) ---
    write_test(test_id, "sharm", "marsh"); test_id += 1
    write_test(test_id, "ananas", "nnaass"); test_id += 1

    # --- БЛОК 2: Маленькие краевые кейсы (3-12) ---
    write_test(test_id, "a", "a"); test_id += 1                   # Один одинаковый символ (YES)
    write_test(test_id, "x", "y"); test_id += 1                   # Один разный символ (NO)
    write_test(test_id, "abc", "aabbcc"); test_id += 1             # Набор символов совпадает, но частоты разные (NO)
    write_test(test_id, "abc", "abcd"); test_id += 1               # Вторая строка длиннее на один символ (NO)
    write_test(test_id, "abcd", "abc"); test_id += 1               # Первая строка длиннее на один символ (NO)
    write_test(test_id, "ab", "ba"); test_id += 1                 # Два символа, перестановка (YES)
    write_test(test_id, "aaaa", "aaaa"); test_id += 1             # Полностью идентичные строки (YES)
    write_test(test_id, "apple", "elppa"); test_id += 1           # Развернутая строка (YES)
    write_test(test_id, "banana", "nabana"); test_id += 1         # Обычная анаграмма (YES)
    write_test(test_id, "abacaba", "abacaba"); test_id += 1       # Палиндром (YES)

    # --- БЛОК 3: Тесты на "почти" анаграммы и специфику частот (13-25) ---
    # Строки, отличающиеся ровно на один символ в разных местах (тесты на внимательность)
    write_test(test_id, "abcdefg", "abcdefz"); test_id += 1
    write_test(test_id, "abcdefg", "zbcdefg"); test_id += 1
    write_test(test_id, "abcdefg", "abczefg"); test_id += 1
    
    # Строки большой длины, состоящие из одной буквы, но разной длины
    write_test(test_id, "a" * 9999, "a" * 10000); test_id += 1
    write_test(test_id, "b" * 10000, "b" * 9999); test_id += 1

    # Случайные маленькие тесты (длина 10-50) с гарантированным результатом
    for _ in range(4):
        length = random.randint(10, 50)
        s1 = "".join(random.choice(alphabet) for _ in range(length))
        s2 = list(s1)
        random.shuffle(s2)
        write_test(test_id, s1, "".join(s2)); test_id += 1 # YES
        
    for _ in range(4):
        length = random.randint(10, 50)
        s1 = "".join(random.choice(alphabet) for _ in range(length))
        s2 = "".join(random.choice(alphabet) for _ in range(length))
        write_test(test_id, s1, s2); test_id += 1 # С высокой вероятностью NO

    # --- БЛОК 4: Средние случайные тесты (26-40) ---
    # Тесты с длиной строк от 1000 до 15000 символов
    for i in range(15):
        length = random.randint(1000, 15000)
        s1 = "".join(random.choice(alphabet) for _ in range(length))
        if i % 2 == 0:
            # Создаем честную анаграмму
            s2_list = list(s1)
            random.shuffle(s2_list)
            write_test(test_id, s1, "".join(s2_list))
        else:
            # Генерируем случайную вторую строку той же длины
            s2 = "".join(random.choice(alphabet) for _ in range(length))
            write_test(test_id, s1, s2)
        test_id += 1

    # --- БЛОК 5: Большие стресс-тесты (41-50) ---
    # Длина строк около 50 000 символов
    for i in range(10):
        length = random.randint(40000, 60000)
        s1 = "".join(random.choice(alphabet) for _ in range(length))
        if i % 2 == 0:
            s2_list = list(s1)
            random.shuffle(s2_list)
            write_test(test_id, s1, "".join(s2_list))
        else:
            s2_list = list(s1)
            s2_list[length // 2] = 'a' if s2_list[length // 2] != 'a' else 'b' # Точечное искажение
            random.shuffle(s2_list)
            write_test(test_id, s1, "".join(s2_list))
        test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (51-60) ---
    # Ровно N = 100 000 символов. Любое квадратичное решение (например, удаление 
    # символов через .replace() или list.remove() в цикле) здесь намертво зависнет по TLE.
    
    # 51-54: Максимальные честные анаграммы (YES)
    for _ in range(4):
        s1 = "".join(random.choice(alphabet) for _ in range(max_len))
        s2_list = list(s1)
        random.shuffle(s2_list)
        write_test(test_id, s1, "".join(s2_list)); test_id += 1

    # 55-58: Почти анаграммы максимальной длины (отличаются ровно на 1 символ в центре) (NO)
    for _ in range(4):
        s1 = "".join(random.choice(alphabet) for _ in range(max_len))
        s2_list = list(s1)
        random.shuffle(s2_list)
        # Вносим минимальное искажение в случайную позицию
        pos = random.randint(0, max_len - 1)
        s2_list[pos] = 'y' if s2_list[pos] != 'y' else 'z'
        write_test(test_id, s1, "".join(s2_list)); test_id += 1

    # 59-60: Полностью случайные несовпадающие строки максимальной длины (NO)
    for _ in range(2):
        s1 = "".join(random.choice(alphabet) for _ in range(max_len))
        s2 = "".join(random.choice(alphabet) for _ in range(max_len))
        write_test(test_id, s1, s2); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, word1, word2):
    """Вспомогательная функция для записи теста в файл формата XX.in"""
    filename = f"tests/{str(test_id).zfill(2)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{word1}\n")
        f.write(f"{word2}\n")

if __name__ == "__main__":
    # Фиксируем seed для стабильной и воспроизводимой генерации
    random.seed(666)
    generate_tests()