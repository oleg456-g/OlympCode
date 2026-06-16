#!/usr/bin/env python3
import random
import os

def generate_tests():
    os.makedirs("tests", exist_ok=True)
    
    test_id = 1
    UPPER_ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

    # --- БЛОК 1: Примеры из условия (1-3) ---
    write_test(test_id, 3, "AAB"); test_id += 1
    write_test(test_id, 6, "QAZQAZ"); test_id += 1
    write_test(test_id, 6, "ABCDEF"); test_id += 1

    # --- БЛОК 2: Минимальные и тривиальные кейсы (4-15) ---
    write_test(test_id, 1, "A"); test_id += 1
    write_test(test_id, 1, "Z"); test_id += 1
    write_test(test_id, 2, "AA"); test_id += 1
    write_test(test_id, 2, "AB"); test_id += 1
    write_test(test_id, 3, "ZZZ"); test_id += 1
    write_test(test_id, 4, "ABCD"); test_id += 1
    write_test(test_id, 4, "BABA"); test_id += 1
    write_test(test_id, 5, "RACEC"); test_id += 1
    
    # Строки, состоящие из одной повторяющейся буквы разной длины
    for length in [7, 12, 25, 50]:
        write_test(test_id, length, "M" * length); test_id += 1

    # --- БЛОК 3: Тесты на структуру палиндрома и выбор середины (16-45) ---
    # Несколько непарных символов, ловящих правильный выбор лексикографически минимального 'mid'
    write_test(test_id, 5, "ZCDAB"); test_id += 1  # Только непарные, ответ 'A'
    write_test(test_id, 7, "AABBXYZ"); test_id += 1  # Парные AA, BB и непарные X, Y, Z -> 'ABXBA'
    write_test(test_id, 6, "ZZAABB"); test_id += 1  # Только парные, нет центрального элемента

    # Генерируем контролируемые комбинации
    for i in range(27):
        # Строим строку, где гарантированно много парных и несколько одиночных
        pairs = "".join([c * 2 for c in random.choices(UPPER_ALPHABET, k=5)])
        odds = "".join(random.sample(UPPER_ALPHABET, k=3))
        s = list(pairs + odds)
        random.shuffle(s)
        write_test(test_id, len(s), "".join(s)); test_id += 1

    # --- БЛОК 4: Асимметричные алфавитные тесты (46-75) ---
    # Проверка на корректность сортировки при обратном порядке или специфических скоплениях букв
    for _ in range(15):
        # Строка из конца алфавита с одной маленькой буквой 'B' или 'C'
        s = ["Z", "Y", "X", "W", "V"] * 4 + [random.choice(["B", "C", "D"])]
        random.shuffle(s)
        write_test(test_id, len(s), "".join(s)); test_id += 1

    for _ in range(15):
        # Строка из начала алфавита, но с тяжелым одиночным центром 'Z'
        s = ["A", "B", "C", "D"] * 5 + ["Z"]
        random.shuffle(s)
        write_test(test_id, len(s), "".join(s)); test_id += 1

    # --- БЛОК 5: Средние случайные тесты (76-100) ---
    for _ in range(25):
        n = random.randint(100, 1000)
        # Ограничиваем подмножество используемых букв для генерации гарантированных пар
        chars = random.choices(UPPER_ALPHABET[:10], k=n)
        write_test(test_id, n, "".join(chars)); test_id += 1

    # --- БЛОК 6: Максимальные стресс-тесты (101-120) ---
    # N = 100 000. Полный спектр заглавных латинских букв.
    max_n = 100000
    
    # 101-105: Случайные строки на полном алфавите
    for _ in range(5):
        s = "".join(random.choices(UPPER_ALPHABET, k=max_n))
        write_test(test_id, max_n, s); test_id += 1

    # 106-110: Все буквы уникальны, насколько это возможно (чередующийся алфавит), n = 100000
    for _ in range(5):
        # Так как уникальных букв всего 26, сделаем длинные цепочки без непосредственных повторений
        base = UPPER_ALPHABET * (max_n // 26)
        base += UPPER_ALPHABET[:max_n % 26]
        # Перемешаем блоками, чтобы не было легких пар подряд без сортировки
        s_list = list(base)
        random.shuffle(s_list)
        write_test(test_id, max_n, "".join(s_list)); test_id += 1

    # 111-115: Почти идеальный палиндром (все элементы парные)
    for _ in range(5):
        half_n = max_n // 2
        half = random.choices(UPPER_ALPHABET, k=half_n)
        s = half + half
        random.shuffle(s)
        write_test(test_id, max_n, "".join(s)); test_id += 1

    # 116-120: Худший случай для некоторых разреженных структур — много 'Z' и одна 'A'
    for _ in range(5):
        s = ["Z"] * (max_n - 1) + ["A"]
        random.shuffle(s)
        write_test(test_id, max_n, "".join(s)); test_id += 1

    print(f"Успешно сгенерировано {test_id - 1} тестов в папке 'tests/'")

def write_test(test_id, n, s):
    """Вспомогательная функция для записи теста"""
    filename = f"tests/{str(test_id).zfill(3)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n}\n")
        f.write(f"{s}\n")

if __name__ == "__main__":
    random.seed(555111)
    generate_tests()