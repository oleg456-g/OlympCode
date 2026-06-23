// Чекер задачи 3: Незнайка и рогейн
// Формат вывода: K, затем K номеров пунктов по возрастанию
// Проверяем: 1) первый пункт = 1, 2) набор достижим за T,
// 3) сумма очков не меньше эталонной, 4) при равных очках — набор не хуже по размеру и лекс.
#include "testlib.h"
#include <vector>
#include <algorithm>
#include <set>
using namespace std;

const long long INF = 2e18;

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    int N, M; long long T;
    N = inf.readInt();
    T = inf.readLong();
    M = inf.readInt();

    vector<long long> p(N);
    for (int i = 0; i < N; i++) p[i] = inf.readLong();

    vector<vector<long long>> G(N, vector<long long>(N));
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) G[i][j] = inf.readLong();

    for (int q = 0; q < M; q++) {
        int u = inf.readInt() - 1;
        int v = inf.readInt() - 1;
        long long d = inf.readLong();
        G[u][v] = min(G[u][v], d);
        G[v][u] = min(G[v][u], d);
    }

    // Заменяем 0 на INF для несмежных (и ставим минимум дорога/лес)
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (i != j && G[i][j] == 0) G[i][j] = INF;

    // Floyd-Warshall
    for (int k = 0; k < N; k++)
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (G[i][k] < INF && G[k][j] < INF)
                    G[i][j] = min(G[i][j], G[i][k] + G[k][j]);

    // Функция проверки набора пунктов (без пункта 1)
    auto checkSet = [&](vector<int>& pts) -> pair<bool, long long> {
        // pts уже без пункта 1; проверяем все перестановки
        // Но N<=10, размер набора <= 9. Для проверки: можно ли за T посетить
        // Реальная проверка — можем ли мы посетить pts за T, начав и закончив в 0
        // Это задача коммивояжёра. Используем bitmask DP.
        int sz = pts.size();
        if (sz == 0) {
            // Только пункт 1
            return {true, p[0]};
        }
        // dp[mask][last] = минимальное время чтобы посетить pts[i] для i в mask,
        // последний — pts[last], начиная из пункта 0
        vector<vector<long long>> dp(1<<sz, vector<long long>(sz, INF));
        for (int i = 0; i < sz; i++) {
            if (G[0][pts[i]] < INF)
                dp[1<<i][i] = G[0][pts[i]];
        }
        for (int mask = 1; mask < (1<<sz); mask++) {
            for (int last = 0; last < sz; last++) {
                if (!(mask & (1<<last))) continue;
                if (dp[mask][last] >= INF) continue;
                for (int nxt = 0; nxt < sz; nxt++) {
                    if (mask & (1<<nxt)) continue;
                    if (G[pts[last]][pts[nxt]] >= INF) continue;
                    int nm = mask | (1<<nxt);
                    dp[nm][nxt] = min(dp[nm][nxt], dp[mask][last] + G[pts[last]][pts[nxt]]);
                }
            }
        }
        int full = (1<<sz)-1;
        long long best = INF;
        for (int last = 0; last < sz; last++) {
            if (G[pts[last]][0] < INF)
                best = min(best, dp[full][last] + G[pts[last]][0]);
        }
        if (best > T) return {false, 0};
        long long score = p[0];
        for (int i = 0; i < sz; i++) score += p[pts[i]];
        return {true, score};
    };

    // Читаем эталон
    int ja_k = ans.readInt();
    vector<int> ja_pts;
    for (int i = 0; i < ja_k; i++) ja_pts.push_back(ans.readInt() - 1);

    vector<int> ja_extra;
    for (int x : ja_pts) if (x != 0) ja_extra.push_back(x);
    auto [ja_ok, ja_score] = checkSet(ja_extra);

    // Читаем ответ участника
    if (ouf.seekEof()) quitf(_wa, "Пустой вывод");
    int pa_k = ouf.readInt(1, N, "K");
    vector<int> pa_pts;
    for (int i = 0; i < pa_k; i++) pa_pts.push_back(ouf.readInt(1, N, "пункт") - 1);

    // Проверки
    if (pa_pts[0] != 0)
        quitf(_wa, "Первый пункт должен быть 1, получено %d", pa_pts[0]+1);

    // Уникальность
    set<int> uniq(pa_pts.begin(), pa_pts.end());
    if ((int)uniq.size() != pa_k)
        quitf(_wa, "Пункты повторяются");

    // Сортировка по возрастанию
    vector<int> sorted_pa = pa_pts;
    sort(sorted_pa.begin(), sorted_pa.end());
    if (sorted_pa != pa_pts)
        quitf(_wa, "Пункты должны быть отсортированы по возрастанию");

    vector<int> pa_extra;
    for (int x : pa_pts) if (x != 0) pa_extra.push_back(x);
    auto [pa_ok, pa_score] = checkSet(pa_extra);

    if (!pa_ok)
        quitf(_wa, "Набор не достижим за время T=%lld", T);

    if (pa_score < ja_score)
        quitf(_wa, "Сумма очков %lld < эталонной %lld", pa_score, ja_score);

    if (pa_score > ja_score)
        quitf(_fail, "Сумма очков участника %lld > эталонной %lld — ошибка в эталоне?", pa_score, ja_score);

    // Очки равны — проверяем размер и лексику
    if (pa_k > ja_k)
        quitf(_wa, "Размер набора %d > эталонного %d при равных очках", pa_k, ja_k);

    if (pa_k < ja_k)
        quitf(_fail, "Размер набора %d < эталонного %d — ошибка в эталоне?", pa_k, ja_k);

    if (pa_pts > ja_pts)
        quitf(_wa, "Лексикографически больший набор при равных очках и размере");

    if (pa_pts < ja_pts)
        quitf(_fail, "Лексикографически меньший набор — ошибка в эталоне?");

    quitf(_ok, "Правильно: %d пунктов, %lld очков", pa_k, pa_score);
}
