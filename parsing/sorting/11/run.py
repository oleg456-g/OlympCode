#!/usr/bin/env python3
import os
import subprocess
import sys
import time

def find_solution_file():
    """Ищет авторское решение в текущей директории."""
    valid_extensions = ['.cpp', '.py']
    files = [f for f in os.listdir('.') if os.path.isfile(f) and os.path.splitext(f)[1] in valid_extensions]
    
    # Исключаем сам скрипт запуска и генератор
    files = [f for f in files if f not in ['run.py', 'gen.py']]
    
    if not files:
        print("[ОШИБКА] Не найдено авторское решение (файл .cpp или .py) в текущей директории!")
        sys.exit(1)
        
    # Если файлов несколько, приоритет отдаем C++, затем Python
    files.sort(key=lambda x: 0 if x.endswith('.cpp') else 1)
    return files[0]

def compile_cpp(source_file):
    """Компилирует C++ файл с оптимизациями."""
    executable = "./solution.exe" if os.name != 'nt' else "solution.exe"
    print(f"[КОМПИЛЯЦИЯ] Компилируем {source_file} с флагом -O3...")
    
    compile_cmd = ["g++", "-O3", "-std=c++17", source_file, "-o", executable]
    
    try:
        result = subprocess.run(compile_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode != 0:
            print("[ОШИБКА] Ошибка компиляции:")
            print(result.stderr)
            sys.exit(1)
        print("[ОК] Компиляция успешна.")
        return executable
    except FileNotFoundError:
        print("[ОШИБКА] Компилятор g++ не найден! Убедитесь, что GCC установлен и добавлен в PATH.")
        sys.exit(1)

def run_tests(executable, is_python):
    """Прогоняет исполняемый файл через все тесты .in и создаёт .out."""
    test_dir = "tests"
    if not os.path.exists(test_dir):
        print(f"[ОШИБКА] Папка '{test_dir}' не найдена. Сначала запустите gen.py!")
        sys.exit(1)
        
    # Получаем список всех .in файлов и сортируем их по имени (01.in, 02.in, ...)
    in_files = sorted([f for f in os.listdir(test_dir) if f.endswith('.in')])
    
    if not in_files:
        print(f"[ОШИБКА] В папке '{test_dir}' нет файлов входных данных .in!")
        sys.exit(1)
        
    print(f"[ЗАПУСК] Найдено {len(in_files)} тестов. Начинаем генерацию ответов...")
    
    for in_file in in_files:
        base_name = os.path.splitext(in_file)[0]
        out_file = f"{base_name}.out"
        
        in_path = os.path.join(test_dir, in_file)
        out_path = os.path.join(test_dir, out_file)
        
        # Настраиваем команду запуска
        if is_python:
            cmd = [sys.executable, executable]
        else:
            cmd = [executable]
            
        start_time = time.time()
        
        # Запускаем процесс, перенаправляя потоки ввода-вывода (справляется с EOF-задачами)
        with open(in_path, "r", encoding="utf-8") as fin, open(out_path, "w", encoding="utf-8") as fout:
            try:
                process = subprocess.run(cmd, stdin=fin, stdout=fout, stderr=subprocess.PIPE, timeout=2.0)
                
                if process.returncode != 0:
                    print(f"  [СТАТУС] Тест {in_file}: Ошибка выполнения (Runtime Error) -> {process.stderr.decode('utf-8', errors='ignore')}")
                else:
                    elapsed = (time.time() - start_time) * 1000
                    print(f"  [УСПЕХ] Тест {in_file} -> {out_file} за {elapsed:.1f} мс")
                    
            except subprocess.TimeoutExpired:
                print(f"  [ТАЙМАУТ] Тест {in_file}: Превышено ограничение времени (Time Limit Exceeded) > 2.0 сек")

    print("\n[ГОТОВО] Все файлы ответов .out успешно сгенерированы в папке 'tests/'!")

def main():
    sol_file = find_solution_file()
    print(f"[ИНФО] Найдено авторское решение: {sol_file}")
    
    if sol_file.endswith('.cpp'):
        executable = compile_cpp(sol_file)
        is_python = False
    else:
        executable = sol_file
        is_python = True
        
    run_tests(executable, is_python)
    
    # Очистка исполняемого файла после работы (для Linux/macOS)
    if not is_python and os.path.exists(executable):
        try:
            os.remove(executable)
        except OSError:
            pass

if __name__ == "__main__":
    main()