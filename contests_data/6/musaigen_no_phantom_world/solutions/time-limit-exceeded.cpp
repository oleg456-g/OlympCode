#include <bits/stdc++.h>

using namespace std;

struct dsu {
    vector<int> p, sz;
    dsu(int n) {
        p.resize(n);
        iota(p.begin(), p.end(), 0);
        sz.resize(n, 1);
    }

    int get(int v) {
        if (p[v] == v)
            return v;
        return p[v] = get(p[v]);
    }

    void merge(int a, int b) {
        a = get(a), b = get(b);
        if (a == b)
            return;
        if (sz[a] > sz[b])
            swap(a, b);
        p[a] = b;
        sz[b] += sz[a];
    }
};

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    int n;
    cin >> n;
    vector<array<int, 3>> tree;
    int q;
    cin >> q;
    while (q--) {
        char type;
        cin >> type;
        if (type == '?') {
            int ind;
            cin >> ind;
            ind = n - ind - 1;
            if (ind == -1) {
                cout << 0 << '\n';
            } else if (tree.size() <= ind) {
                cout << -1 << '\n';
            } else {
                cout << tree[ind][0] << '\n';
            }
        } else {
            int a, b, c;
            cin >> a >> b >> c;
            --a, --b;
            dsu d(n);
            vector<array<int, 3>> new_tree;
            for (auto i : tree) {
                if (i[0] > c) {
                    if (d.get(a) != d.get(b)) {
                        d.merge(a, b);
                        new_tree.push_back({c, a, b});
                    }
                }
                if (d.get(i[1]) != d.get(i[2])) {
                    d.merge(i[1], i[2]);
                    new_tree.push_back(i);
                }
            }
            if (!tree.size() || tree.back()[0] <= c) {
                if (d.get(a) != d.get(b)) {
                    d.merge(a, b);
                    new_tree.push_back({c, a, b});
                }
            }
            tree = new_tree;
        }
    }
}