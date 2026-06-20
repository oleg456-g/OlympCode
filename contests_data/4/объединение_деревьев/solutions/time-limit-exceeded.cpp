#include <bits/stdc++.h>

using namespace std;
using ll = long long;

ll xor_sum = 0;

void dfs(int x, vector<bool> &used, vector<int> &xr, const vector<int> &w, const vector<vector<int>> &g) {
//    cout << "dfs " << x << "\n";
    used[x] = true;
    if (g[x].empty()) xor_sum += (xr[x] * 1LL);

    for (auto &i: g[x]) {
//        cout << "neigh " << i << "\n";
        xr[i] = w[i] ^ xr[x];
        dfs(i, used, xr, w, g);
    }
}

ll search(int n1, int n2, int r1, int r2, vector<int> w1, vector<int> w2, vector<vector<int>> g1,
           vector<vector<int>> g2) {
    //combined graph
    for (int i = 0; i < n2; i++) {
        g1.emplace_back();
        w1.emplace_back(w2[i]);
        for (auto &j: g2[i]) {
            g1[i + n1].emplace_back(j + n1);
        }
    }

    ll best = 0;
    const int sz = n1 + n2 + 1;

    for (int i = 0; i < n1; i++) {
        vector<bool> used(sz, false);
        vector<int> xr(sz, 0);
        xor_sum = 0;

        xr[r1] = w1[r1];
        g1[i].emplace_back(r2 + n1);
//        cout << "going to call dfs\n";
        dfs(r1, used, xr, w1, g1);
        best = max(xor_sum, best);
        g1[i].pop_back();
    }
    return best;
}

pair<vector<vector<int>>, int> make_graph(int n, vector<int> &p) {
    vector<vector<int>> g(n + 1);
    int root = -1;
    for (int i = 0; i < n; i++) {
        if (p[i] == i) {
            root = i;
        } else {
            g[p[i]].emplace_back(i);
        }
    }
    return {g, root};
}

void solve() {
    int n1;
    cin >> n1;
    vector<int> w1(n1), p1(n1);
    for (auto &i: w1) cin >> i;
    for (auto &i: p1) {
        cin >> i;
        i--;
    }
    int n2;
    cin >> n2;
    vector<int> w2(n2), p2(n2);

    for (auto &i: w2) cin >> i;

    for (auto &i: p2) {
        cin >> i;
        i--;
    }

    auto [g1, r1] = make_graph(n1, p1);
    auto [g2, r2] = make_graph(n2, p2);

    ll sum1 = search(n1, n2, r1, r2, w1, w2, g1, g2);
    ll sum2 = search(n2, n1, r2, r1, w2, w1, g2, g1);

    cout << max(sum1, sum2) << "\n";


}

int main() {
    int t;
    cin >> t;
    while (t--) solve();
}