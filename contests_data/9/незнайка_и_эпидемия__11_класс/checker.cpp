// Чекер задачи 5: Незнайка и эпидемия
// Для каждого запроса выводится одно число — длина совпадающего фрагмента
// Вывод детерминирован
#include "testlib.h"
using namespace std;

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    // Из входных данных читаем K — количество запросов
    int M = inf.readInt(); // длина РНК
    int N = inf.readInt(); // количество пациентов
    int K = inf.readInt(); // количество запросов
    // Пропускаем N-1 контактов
    for (int i = 0; i < N - 1; i++) {
        inf.readInt(); inf.readInt(); inf.readInt();
    }
    // Пропускаем K запросов
    for (int i = 0; i < K; i++) {
        inf.readInt(); inf.readInt();
    }

    for (int q = 1; q <= K; q++) {
        if (ans.seekEof())
            quitf(_fail, "Эталон содержит меньше %d ответов", q);

        long long ja = ans.readLong();

        if (ouf.seekEof())
            quitf(_wa, "Недостаточно ответов: ожидается ответ на запрос %d", q);

        long long pa = ouf.readLong();

        if (pa != ja)
            quitf(_wa, "Запрос %d: ожидается %lld, получено %lld", q, ja, pa);
    }

    if (!ouf.seekEof())
        quitf(_wa, "Лишние числа в выводе");

    quitf(_ok, "Правильно: %d ответов", K);
}
