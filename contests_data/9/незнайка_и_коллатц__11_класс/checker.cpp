// Чекер задачи 1: Незнайка и Коллатц
// Верифицируем путь математически: каждый переход должен быть
// либо f(x) = x/2 или 3x+1, либо f^-1(x) — т.е. x*2 или (x-1)/3
#include "testlib.h"
#include <vector>
using namespace std;

// Функция Коллатца f(x)
long long f(long long x) {
    if (x % 2 == 0) return x / 2;
    return 3 * x + 1;
}

// Проверяем что b достижима из a за один шаг (f или f^-1)
bool oneStep(long long a, long long b) {
    if (f(a) == b) return true;          // a -> f(a)
    if (f(b) == a) return true;          // f(b) = a, т.е. a = f^-1(b)
    return false;
}

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    // Читаем входные данные
    long long n1 = inf.readLong();
    long long n2 = inf.readLong();

    // Читаем ответ эталона (для минимальности)
    int ja_steps = ans.readInt();

    // Читаем ответ участника
    if (ouf.seekEof())
        quitf(_wa, "Пустой вывод");

    int pa_steps = ouf.readInt(-1, 2000000, "количество шагов");

    if (ja_steps == -1) {
        if (pa_steps != -1)
            quitf(_wa, "Ожидается -1 (нет пути), получено %d", pa_steps);
        quitf(_ok, "Правильно: -1");
    }

    if (pa_steps == -1)
        quitf(_wa, "Ожидается %d шагов, получено -1", ja_steps);

    if (pa_steps < ja_steps)
        quitf(_fail, "Участник нашёл путь короче эталона (%d < %d) — ошибка эталона?",
              pa_steps, ja_steps);

    if (pa_steps > ja_steps)
        quitf(_wa, "Количество шагов %d больше минимального %d", pa_steps, ja_steps);

    // pa_steps == ja_steps, теперь проверяем сам путь
    if (pa_steps == 0) {
        if (n1 != n2)
            quitf(_wa, "Вывод 0 шагов, но n1=%lld != n2=%lld", n1, n2);
        quitf(_ok, "Правильно: 0 шагов");
    }

    if (pa_steps == 1) {
        if (!oneStep(n1, n2))
            quitf(_wa, "Нет прямого перехода за 1 шаг между %lld и %lld", n1, n2);
        quitf(_ok, "Правильно: 1 шаг");
    }

    // pa_steps >= 2: читаем промежуточные числа
    if (ouf.seekEof())
        quitf(_wa, "Ожидается строка с %d промежуточными числами", pa_steps - 1);

    vector<long long> path;
    path.push_back(n1);
    for (int i = 0; i < pa_steps - 1; i++) {
        if (ouf.seekEof() || ouf.eoln())
            quitf(_wa, "Ожидается %d промежуточных чисел, найдено %d",
                  pa_steps - 1, i);
        long long x = ouf.readLong(1, (long long)1e15, "промежуточное число");
        path.push_back(x);
    }
    path.push_back(n2);

    // Проверяем каждый переход
    for (int i = 0; i + 1 < (int)path.size(); i++) {
        if (!oneStep(path[i], path[i+1]))
            quitf(_wa, "Недопустимый переход на шаге %d: %lld -> %lld",
                  i+1, path[i], path[i+1]);
    }

    quitf(_ok, "Правильно: %d шагов", pa_steps);
}
