#include <bits/stdc++.h>

using namespace std;
using ll = long long;

const int BITS = 30;

void dfs(int x, vector<int> &xr, const vector<int> &w, const vector<vector<int>> &g) {
    for (auto &i: g[x]) {
        xr[i] = w[i] ^ xr[x];
        dfs(i, xr, w, g);
    }
}

pair<vector<array<int, 2>>, vector<int>> build_counts(int n, int r, vector<int> &w, vector<vector<int>> &g) {
    vector<array<int, 2>> counts(BITS, {0, 0});

    vector<int> xr(n, 0);
    xr[r] = w[r];
    dfs(r, xr, w, g);

    vector<int> leaves;
    for (int i = 0; i < n; i++) {
        if (g[i].empty()) leaves.emplace_back(xr[i]);
    }
    for (auto &num: leaves) {
        for (int i = 0; i < BITS; ++i) {
            if (num & (1 << i)) {
                counts[i][1]++;
            } else {
                counts[i][0]++;
            }
        }
    }

    return {counts, xr};
}

ll sum_from_counts(const vector<array<int, 2>> &counts) {
    ll sum = 0;
    for (int i = 0; i < BITS; ++i) {
        sum += (1LL << i) * counts[i][1];
    }
    return sum;
}

void xor_counts(vector<array<int, 2>> &counts, int y) {
    for (int i = 0; i < BITS; ++i) {
        if (y & (1 << i)) {
            swap(counts[i][0], counts[i][1]);
        }
    }
}

ll search(int n1, const ll sum1, const vector<int> &xr1, const vector<vector<int>> &g1, vector<array<int, 2>>&counts2) {
    ll best = 0;
    for (int i = 0; i < n1; i++) {
        xor_counts(counts2, xr1[i]);
        ll sum = sum_from_counts(counts2) + sum1;
        if (g1[i].empty()) sum -= xr1[i];
        best = max(best, sum);
        xor_counts(counts2, xr1[i]);
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

    auto [counts1, xr1] = build_counts(n1, r1, w1, g1);
    auto [counts2, xr2] = build_counts(n2, r2, w2, g2);

    ll sum1 = sum_from_counts(counts1);
    ll sum2 = sum_from_counts(counts2);

    ll res1 = search(n1, sum1, xr1, g1, counts2);
    ll res2 = search(n2, sum2, xr2, g2, counts1);

    cout << max(res1, res2) << "\n";
}

int main() {
    int t;
    cin >> t;
    while (t--) solve();
}