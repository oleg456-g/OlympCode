#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <set>
#include <algorithm>

using namespace std;

const int MAXN = 300'000;
const int LOG = 19;
int h[MAXN];
int up[LOG][MAXN];
int parent[MAXN];
int c = 1;

int go_up(int u, int k) {
    for (int i = LOG - 1; i >= 0; --i) {
        if (k >= (1 << i)) {
            k -= (1 << i);
            u = up[i][u];
        }
    }
    return u;
}

int lca(int u, int v) {
    if (h[u] < h[v]) {
        swap(u, v);
    }
    u = go_up(u, h[u] - h[v]);
    if (u == v) {
        return u;
    }
    for (int i = LOG - 1; i >= 0; --i) {
        if (up[i][u] != up[i][v]) {
            u = up[i][u];
            v = up[i][v];
        }
    }
    return up[0][u];
}

int dist(int u, int v) {
    return h[u] + h[v] - 2 * h[lca(u, v)];
}

pair<int, int> result = { 1, 0 };

void add_vertex(int p, int v) {
    h[v] = h[p] + 1;
    parent[v] = p;
    up[0][v] = p;
    for (int i = 1; i < LOG; ++i) {
        up[i][v] = up[i - 1][up[i - 1][v]];
    }
    if (h[v] > result.first) {
        result = { h[v], 1 };
    } else if (h[v] == result.first) {
        ++result.second;
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    int q, m;
    cin >> q >> m;
    int answ = 0;
    while (q--) {
        char t;
        int v;
        cin >> t >> v;
        if (m == 0) {
            v = (v - 1) % c + 1;
        } else {
            if (answ) {
                v = ((answ + (v - 1)) % c + c) % c + 1;
            }
        }
        if (t == '+') {
            add_vertex(v - 1, c);
            ++c;
        } else {
            auto [x, y] = result;
            answ = x + y;
            cout << x << ' ' << y << '\n';
        }
    }
}
