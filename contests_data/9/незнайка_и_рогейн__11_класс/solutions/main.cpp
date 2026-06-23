#include <bits/stdc++.h>

using namespace std;

const int inf = 1'000'000'001;

int main() {
    // read data
    int N, T, M;
    cin >> N >> T >> M;
    vector<int> p(N);
    for (int i = 0; i < N; ++i) cin >> p[i];
    vector<vector<int>> G(N, vector<int>(N));
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) cin >> G[i][j];
    int u, v, d;
    for (int i = 0; i < M; ++i) {
        cin >> u >> v >> d;
        u--;
        v--;
        G[u][v] = min(G[u][v], d);
        G[v][u] = min(G[v][u], d);
    }
    // preprocessing
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            if (i == j) continue;
            if (G[i][j] == 0) G[i][j] = inf;
        }

    for (int k = 0; k < N; ++k)
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                G[i][j] = min(G[i][j], G[i][k] + G[k][j]);

    // main algorithm
    vector<int> route;
    for (int i = 1; i < N; ++i) route.push_back(i);
    long long max_score = p[0];
    vector<int> max_route;
    do {
        long long iter_score = p[0];
        long long iter_time = 0;
        int iter_pos = 0;
        for (int checkpoint : route) {
            if (G[iter_pos][checkpoint] >= inf) continue;
            if (G[checkpoint][0] >= inf) continue;
            if (iter_time + G[iter_pos][checkpoint] + G[checkpoint][0] <= T) {
                iter_time += G[iter_pos][checkpoint];
                iter_score += p[checkpoint];
                iter_pos = checkpoint;
            } else
                break;
        }
        if (max_score < iter_score) {
            max_score = iter_score;
            max_route.clear();
            for (int checkpoint : route) {
                if (checkpoint == iter_pos) {
                    max_route.push_back(checkpoint);
                    break;
                }
                max_route.push_back(checkpoint);
            }
        }
    } while (next_permutation(route.begin(), route.end()));
    // print answer
    set<int> ans = {1};
    for (int checkpoint : max_route) ans.insert(checkpoint + 1);
    cout << ans.size() << endl;
    for (int checkpoint : ans) cout << checkpoint << ' ';
    cout << endl;
}