#include <bits/stdc++.h>
using namespace std;

static const int INF = (int)1e9;

int id(char c) {
    if (c == 'U') return 0;
    if (c == 'C') return 1;
    if (c == 'A') return 2;
    return 3;
}

bool is_stop(const array<int, 3>& c) {
    return c[0] == id('U') && (
        (c[1] == id('A') && c[2] == id('A')) ||
        (c[1] == id('A') && c[2] == id('G')) ||
        (c[1] == id('G') && c[2] == id('A'))
    );
}

void solve() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, k;
    string s;
    cin >> n >> k >> s;

    int m = (int)s.size();

    vector<array<int, 4>> nxt(m + 2);
    for (int c = 0; c < 4; c++) nxt[m][c] = nxt[m + 1][c] = m;
    for (int i = m - 1; i >= 0; i--) {
        nxt[i] = nxt[i + 1];
        nxt[i][id(s[i])] = i;
    }

    vector<array<int, 3>> mids, stops, augs;
    array<int, 3> aug = {id('A'), id('U'), id('G')};
    augs.push_back(aug);

    string alph = "UCAG";
    for (char a : alph) {
        for (char b : alph) {
            for (char c : alph) {
                array<int, 3> cur = {id(a), id(b), id(c)};
                if (is_stop(cur)) stops.push_back(cur);
                else mids.push_back(cur);
            }
        }
    }

    auto need_one = [&](int p, const array<int, 3>& cod) -> int {
        if (p >= m) return INF;
        int p1 = nxt[p][cod[0]];
        if (p1 >= m) return INF;
        int p2 = nxt[p1 + 1][cod[1]];
        if (p2 >= m) return INF;
        int p3 = nxt[p2 + 1][cod[2]];
        if (p3 >= m) return INF;
        return p3 - p - 2;
    };

    auto need_list = [&](int p, const vector<array<int, 3>>& list) -> int {
        int res = INF;
        for (const auto& cod : list) {
            res = min(res, need_one(p, cod));
        }
        return res;
    };

    vector<int> needAug(m + 1, INF), needMid(m + 1, INF), needStop(m + 1, INF);
    for (int p = 0; p <= m; p++) {
        needAug[p] = need_list(p, augs);
        needMid[p] = need_list(p, mids);
        needStop[p] = need_list(p, stops);
    }

    if (n < 2) {
        for (int i = 1; i <= k; i++) {
            if (i > 1) cout << ' ';
            cout << i;
        }
        cout << '\n';
        return;
    }

    vector<int> L(n + 1, INF);
    L[0] = 0;

    if (needAug[0] > k) {
        for (int i = 1; i <= k; i++) {
            if (i > 1) cout << ' ';
            cout << i;
        }
        cout << '\n';
        return;
    }

    L[1] = needAug[0];

    for (int j = 1; j <= n - 2; j++) {
        int best = INF;
        int base = 3 * j;
        for (int d = L[j]; d <= k; d++) {
            int p = base + d;
            if (p <= m) best = min(best, d + needMid[p]);
        }
        L[j + 1] = best;
        if (L[j + 1] > k) break;
    }

    if (L[n - 1] <= k) {
        int best = INF;
        int base = 3 * (n - 1);
        for (int d = L[n - 1]; d <= k; d++) {
            int p = base + d;
            if (p <= m) best = min(best, d + needStop[p]);
        }
        L[n] = best;
    }

    if (L[n] > k) {
        for (int i = 1; i <= k; i++) {
            if (i > 1) cout << ' ';
            cout << i;
        }
        cout << '\n';
        return;
    }

    vector<int> D(n + 1, 0);
    D[n] = k;

    for (int j = n - 1; j >= 1; j--) {
        bool ok = false;
        int low = L[j];
        int high = D[j + 1];
        for (int d = high; d >= low; d--) {
            int p = 3 * j + d;
            int need = (j == n - 1 ? needStop[p] : needMid[p]);
            if (need <= D[j + 1] - d) {
                D[j] = d;
                ok = true;
                break;
            }
        }
        if (!ok) {
            for (int i = 1; i <= k; i++) {
                if (i > 1) cout << ' ';
                cout << i;
            }
            cout << '\n';
            return;
        }
    }

    if (needAug[0] > D[1]) {
        for (int i = 1; i <= k; i++) {
            if (i > 1) cout << ' ';
            cout << i;
        }
        cout << '\n';
        return;
    }

    auto find_pos = [&](int p, int delta, const vector<array<int, 3>>& list) -> array<int, 3> {
        int lim = p + delta + 2;
        array<int, 3> best = {-1, -1, -1};
        int bestEnd = INF;
        for (const auto& cod : list) {
            int p1 = nxt[p][cod[0]];
            if (p1 >= m || p1 > lim) continue;
            int p2 = nxt[p1 + 1][cod[1]];
            if (p2 >= m || p2 > lim) continue;
            int p3 = nxt[p2 + 1][cod[2]];
            if (p3 >= m || p3 > lim) continue;
            if (p3 < bestEnd) {
                bestEnd = p3;
                best = {p1, p2, p3};
            }
        }
        return best;
    };

    vector<int> ans;
    ans.reserve(k);

    for (int j = 0; j < n; j++) {
        int d0 = D[j];
        int d1 = D[j + 1];
        int p = 3 * j + d0;
        int delta = d1 - d0;
        int r = p + 3 + delta - 1;

        array<int, 3> keep;
        if (j == 0) keep = find_pos(p, delta, augs);
        else if (j == n - 1) keep = find_pos(p, delta, stops);
        else keep = find_pos(p, delta, mids);

        if (keep[0] == -1) {
            for (int i = 1; i <= k; i++) {
                if (i > 1) cout << ' ';
                cout << i;
            }
            cout << '\n';
            return;
        }

        for (int x = p; x <= r; x++) {
            if (x != keep[0] && x != keep[1] && x != keep[2]) {
                ans.push_back(x + 1);
            }
        }
    }

    if ((int)ans.size() != k) {
        for (int i = 1; i <= k; i++) {
            if (i > 1) cout << ' ';
            cout << i;
        }
        cout << '\n';
        return;
    }

    for (int i = 0; i < k; i++) {
        if (i) cout << ' ';
        cout << ans[i];
    }
    cout << '\n';
    return;
}

int main() {
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}