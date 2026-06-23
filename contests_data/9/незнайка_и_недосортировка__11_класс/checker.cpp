// Чекер задачи 2: Незнайка и недосортировка
// Верифицирует ответ математически: после обмена A[i1] и A[i2]
// последовательность должна быть строго убывающей.
#include "testlib.h"
#include <vector>
#include <string>
using namespace std;

typedef vector<int> Number;

// Разбирает запись числа в нормализованный массив 50 разрядов (старший — первый).
// Алгоритм точно соответствует Pascal-решению из условия.
Number parseNumber(const string& s) {
    // Шаг 1: читаем разряды слева направо в a[0..j-1] (a[0] — старший)
    vector<int> a(50, 0);
    int j = 0;  // количество разрядов
    for (int i = 0; i < (int)s.size() && j < 50; ) {
        char ch = s[i];
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            int val = (ch >= 'A' && ch <= 'Z')
                      ? ch - 'A'
                      : ch - 'a' + 26;
            a[j] = val;
            i++;
            // Модификатор сразу после буквы
            if (i < (int)s.size()) {
                if      (s[i] == '^') { a[j] += 52;  i++; }
                else if (s[i] == '~') { a[j] += 104; i++; }
                else if (s[i] == '_') { a[j] += 156; i++; }
            }
            j++;
        } else {
            i++;
        }
    }

    // Шаг 2: выравниваем к правому краю (a[0] → b[50-j], ..., a[j-1] → b[49])
    Number b(50, 0);
    for (int k = 0; k < j; k++)
        b[50 - j + k] = a[k];

    // Шаг 3: нормализация переносов справа налево
    for (int k = 49; k > 0; k--) {
        b[k - 1] += b[k] / 52;
        b[k]     %= 52;
    }
    return b;
}

bool strictlyDescending(const vector<Number>& nums) {
    for (int i = 0; i + 1 < (int)nums.size(); i++)
        if (nums[i] <= nums[i + 1]) return false;
    return true;
}

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    int n = inf.readInt();
    vector<string> strs(n);
    for (int i = 0; i < n; i++) strs[i] = inf.readToken();

    vector<Number> nums(n);
    for (int i = 0; i < n; i++) nums[i] = parseNumber(strs[i]);

    // Эталон (для сообщения об ошибке)
    int ja_i1 = ans.readInt();
    int ja_i2 = ans.readInt();

    // Ответ участника
    if (ouf.seekEof())
        quitf(_wa, "Пустой вывод");

    int pa_i1 = ouf.readInt(1, n, "i1");
    int pa_i2 = ouf.readInt(1, n, "i2");

    if (pa_i1 >= pa_i2)
        quitf(_wa, "Требуется i1 < i2, получено %d %d", pa_i1, pa_i2);

    // Верифицируем: после обмена последовательность строго убывает?
    vector<Number> check = nums;
    swap(check[pa_i1 - 1], check[pa_i2 - 1]);
    if (!strictlyDescending(check))
        quitf(_wa, "После обмена %d и %d последовательность не является убывающей",
              pa_i1, pa_i2);

    quitf(_ok, "Правильно: %d %d", pa_i1, pa_i2);
}
