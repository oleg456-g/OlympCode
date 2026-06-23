// Чекер задачи 4: Незнайка и химия
// Вывод детерминирован по условию (минимальные коэффициенты, строгий порядок)
// Сравниваем строку за строкой
#include "testlib.h"
#include <string>
using namespace std;

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    int line = 0;
    while (!ans.seekEof()) {
        line++;
        string ja_line = ans.readLine();
        if (ouf.seekEof())
            quitf(_wa, "Недостаточно строк: ожидается строка %d", line);
        string pa_line = ouf.readLine();

        if (pa_line != ja_line)
            quitf(_wa, "Строка %d: ожидается '%s', получено '%s'",
                  line, ja_line.c_str(), pa_line.c_str());
    }

    if (!ouf.seekEof())
        quitf(_wa, "Лишние строки в выводе после строки %d", line);

    quitf(_ok, "Правильно: %d формул", line);
}
