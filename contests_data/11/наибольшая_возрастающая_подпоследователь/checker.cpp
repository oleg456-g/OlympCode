// Универсальный токенный чекер (для детерминированного вывода)
#include "testlib.h"
using namespace std;
int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);
    int line = 0;
    while (!ans.seekEof()) {
        line++;
        string ja = ans.readLine();
        if (ouf.seekEof()) quitf(_wa, "Мало строк: ожидается строка %d", line);
        string pa = ouf.readLine();
        if (pa != ja) quitf(_wa, "Строка %d: ожидается '%s', получено '%s'",
                            line, ja.c_str(), pa.c_str());
    }
    if (!ouf.seekEof()) quitf(_wa, "Лишние строки после строки %d", line);
    quitf(_ok, "OK: %d строк", line);
}
