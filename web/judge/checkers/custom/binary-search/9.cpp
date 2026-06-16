#include <iostream>
#include <vector>
#include "testlib.h"

using namespace std;

// Глобальные переменные входных данных
int m, n;

struct Helper {
    long long t, z, y;
};
vector<Helper> helpers;

// Функция валидации ответа (паттерн сертификата)
long long readAnswer(InStream& stream, TResult structural_error, TResult logical_error) {
    // 1. Читаем заявленное минимальное время
    long long declared_time = stream.readLong();
    
    if (declared_time < 0) {
        quitf(structural_error, "Time cannot be negative: %s", vtos(declared_time).c_str());
    }

    // 2. Читаем сертификат (распределение шариков)
    long long total_balloons = 0;
    for (int i = 0; i < n; i++) {
        long long b_count = stream.readLong(0LL, (long long)m, "balloon count");
        total_balloons += b_count;

        // 3. Проверяем локальную физическую достижимость
        if (b_count > 0) {
            long long full_cycles = (b_count - 1) / helpers[i].z;
            long long rem_balloons = b_count - full_cycles * helpers[i].z;
            
            long long required_time = full_cycles * (helpers[i].t * helpers[i].z + helpers[i].y) 
                                      + (rem_balloons * helpers[i].t);
            
            if (required_time > declared_time) {
                quitf(logical_error, "Helper %d needs %s minutes to blow up %s balloons, but declared time is %s", 
                      i + 1, vtos(required_time).c_str(), vtos(b_count).c_str(), vtos(declared_time).c_str());
            }
        }
    }

    // 4. Проверяем глобальную сумму (ровно M шариков)
    if (total_balloons != m) {
        quitf(logical_error, "Total balloons inflated (%s) does not equal M (%s)", 
              vtos(total_balloons).c_str(), vtos(m).c_str());
    }

    return declared_time;
}

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    // Читаем входные данные один раз из inf
    m = inf.readInt();
    n = inf.readInt();

    helpers.resize(n);
    for (int i = 0; i < n; i++) {
        helpers[i].t = inf.readInt();
        helpers[i].z = inf.readInt();
        helpers[i].y = inf.readInt();
    }

    // Читаем и валидируем ответ жюри
    long long ja = readAnswer(ans, _fail, _fail);
    
    // ПРАВКА ПО ФИДБЕКУ: Проверяем, что в файле жюри нет лишнего мусора
    ans.skipBlanks();
    ans.readEof();

    // Читаем и валидируем ответ участника
    long long pa = readAnswer(ouf, _pe, _wa);

    // Сравниваем результаты двух корректных сертификатов
    if (ja < pa) {
        quitf(_wa, "Jury has better time: %s < %s", vtos(ja).c_str(), vtos(pa).c_str());
    }

    if (ja > pa) {
        quitf(_fail, "Participant has better time than jury: %s < %s. FIX JURY SOLUTION!", 
              vtos(pa).c_str(), vtos(ja).c_str());
    }

    // Лояльность к пробелам участника
    ouf.skipBlanks();
    ouf.readEof();

    quitf(_ok, "Correct answer. Optimal time: %s, M: %s, N: %d", 
          vtos(ja).c_str(), vtos(m).c_str(), n);
}