#!/usr/bin/env python3
import os
import subprocess
import glob

# Конфигурация: укажи имя файла с авторским решением
SOLUTION_FILE = "solution.cpp"  # или "solution.py"
TESTS_DIR = "tests"

def compile_cpp(source_file):
    """Компилирует C++ файл с оптимизацией O3"""
    executable = "./solution"
    if os.name == 'nt':  # Если запускается на Windows
        executable = "solution.exe"
        
    print(f"Компиляция {source_file}...")
    # -O3 важен для олимпиадных задач, чтобы код работал с максимальной скоростью
    result = subprocess.run(["g++", "-O3", source_file, "-o", executable], 
                            capture_output=True, text=True)
    
    if result.returncode != 0:
        print("Ошибка компиляции!")
        print(result.stderr)
        return None
    return executable

def generate_outputs():
    if not os.path.exists(TESTS_DIR):
        print(f"Ошибка: Папка '{TESTS_DIR}' не найдена. Сначала запусти gen.py!")
        return

    # Определяем тип решения и команду для запуска
    if SOLUTION_FILE.endswith(".cpp"):
        exe = compile_cpp(SOLUTION_FILE)
        if not exe:
            return
        run_command = [exe]
    elif SOLUTION_FILE.endswith(".py"):
        run_command = ["python3", SOLUTION_FILE]
    else:
        print("Ошибка: Поддерживаются только файлы .cpp и .py")
        return

    # Находим все файлы .in в папке тестов
    input_files = sorted(glob.glob(os.path.join(TESTS_DIR, "*.in")))
    
    if not input_files:
        print(f"В папке '{TESTS_DIR}' не найдено файлов .in")
        return

    print(f"Найдено тестов: {len(input_files)}. Запуск генерации ответов...")

    for in_path in input_files:
        # Формируем имя для .out файла (заменяем расширение)
        out_path = in_path.rsplit(".", 1)[0] + ".out"
        
        # Открываем .in для чтения и .out для записи
        with open(in_path, "r") as infile, open(out_path, "w") as outfile:
            try:
                # Запускаем авторское решение, перенаправляя потоки ввода-вывода
                subprocess.run(run_command, stdin=infile, stdout=outfile, check=True)
                print(f"Создан: {out_path}")
            except subprocess.CalledProcessError:
                print(f"Ошибка при выполнении теста {in_path}!")
                
    # Чистим за собой скомпилированный бинарник (если это был C++)
    if SOLUTION_FILE.endswith(".cpp") and os.path.exists(exe):
        os.remove(exe)
        
    print("\nВсе файлы .out успешно сгенерированы!")

if __name__ == "__main__":
    generate_outputs()