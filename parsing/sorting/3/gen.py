#!/usr/bin/env python3
import random
import os

def generate_tests():
    # Создаем папку для тестов, если её нет
    os.makedirs("tests", exist_ok=True)
    
    # 1. Тест из условия задачи (всегда полезно иметь под номером 1)
    write_test(1, 5, [5, 4, 3, 2, 1])
    
    # 2. Краевой кейс: минимальный размер массива (n = 1)
    write_test(2, 1, [42])
    
    # 3. Краевой кейс: n = 2, элементы уже отсортированы
    write_test(3, 2, [10, 20])
    
    # 4. Краевой кейс: n = 2, элементы в обратном порядке
    write_test(4, 2, [20, 10])
    
    # 5. Краевой кейс: все элементы одинаковые (проверка на зацикливание или некорректные знаки >=)
    write_test(5, 10, [7] * 10)
    
    # 6. Краевой кейс: массив уже отсортирован по неубыванию
    write_test(6, 15, list(range(-10, 5)))
    
    # 7. Краевой кейс: массив отсортирован в обратном порядке (максимальное число перестановок)
    write_test(7, 15, list(range(10, -5, -1)))
    
    # 8. Максимальный тест (n = 1000), элементы на границах ограничений [-1000, 1000]
    max_n = 1000
    max_test = [random.randint(-1000, 1000) for _ in range(max_n)]
    write_test(8, max_n, max_test)
    
    # 9-15. Случайные тесты разного размера
    for test_num in range(9, 16):
        n = random.randint(10, 500)
        elements = [random.randint(-1000, 1000) for _ in range(n)]
        write_test(test_num, n, elements)
        
    print(f"Успешно сгенерировано {15} тестов в папке 'tests/'")

def write_test(test_id, n, array):
    """Вспомогательная функция для записи теста в файл формата XX.in"""
    filename = f"tests/{str(test_id).zfill(2)}.in"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"{n}\n")
        f.write(" ".join(map(str, array)) + "\n")

if __name__ == "__main__":
    # Фиксируем seed, чтобы генерация была воспроизводимой при каждом запуске
    random.seed(42)
    generate_tests()