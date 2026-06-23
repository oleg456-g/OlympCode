#include <bits/stdc++.h>

using namespace std;

struct dsu {
    vector<int> p, sz;
    vector<pair<int, int>> upd_p;
    vector<pair<int, int>> upd_sz;
    int count_comp;
    dsu(int n) {
        p.resize(n);
        count_comp = n;
        iota(p.begin(), p.end(), 0);
        sz.resize(n, 1);
    }

    int get(int v) {
        if (p[v] == v)
            return v;
        return get(p[v]);
    }

    void merge(int a, int b) {
        a = get(a), b = get(b);
        if (a == b)
            return;
        --count_comp;
        if (sz[a] > sz[b])
            swap(a, b);
        upd_p.emplace_back(a, p[a]);
        upd_sz.emplace_back(b, sz[b]);
        p[a] = b;
        sz[b] += sz[a];
    }
};

const int B = 433;

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    int n, q;
    cin >> n >> q;
    vector<dsu> d((q + B - 1) / B + 2, dsu(n));
    vector<array<int, 3>> edges;
    vector<int> used(q);
    vector<array<int, 4>> queries(q);
    for (int i = 0; i < q; ++i) {
        char type;
        cin >> type;
        if (type == '?') {
            int ind;
            cin >> ind;
            queries[i] = {0, ind, 0, 0};
        } else {
            int a, b, c;
            cin >> a >> b >> c;
            --a, --b;
            queries[i] = {1, a, b, c};
            edges.push_back({c, a, b});
        }
    }
    sort(edges.begin(), edges.end());
    for (auto Q : queries) {
        if (Q[0] == 0) {
            if (d.back().count_comp > Q[1]) {
                cout << -1 << '\n';
                continue;
            }
            int ind = d.size() - 1;
            while (ind > 0 && d[ind].count_comp <= Q[1]) {
                --ind;
            }
            int sz1 = d[ind].upd_p.size();
            int sz2 = d[ind].upd_sz.size();
            int it = B * ind;
            while (d[ind].count_comp > Q[1]) {
                if (used[it]) {
                    d[ind].merge(edges[it][1], edges[it][2]);
                }
                ++it;
            }
            while (d[ind].upd_p.size() > sz1) {
                d[ind].count_comp++;
                d[ind].p[d[ind].upd_p.back().first] = d[ind].upd_p.back().second;
                d[ind].upd_p.pop_back();
            }
            while (d[ind].upd_sz.size() > sz2) {
                d[ind].sz[d[ind].upd_sz.back().first] = d[ind].upd_sz.back().second;
                d[ind].upd_sz.pop_back();
            }
            if (it == 0) {
                cout << 0 << '\n';
            } else {
                cout << edges[it - 1][0] << '\n';
            }
        } else {
            int ind_edge = lower_bound(edges.begin(), edges.end(), 
                                array<int, 3> {Q[3], Q[1], Q[2]}) - edges.begin();
            used[ind_edge] = 1;
            for (int i = 0; i < d.size(); ++i) {
                if (i * B > ind_edge) {
                    d[i].merge(Q[1], Q[2]);
                }
            }
        }
    }
}