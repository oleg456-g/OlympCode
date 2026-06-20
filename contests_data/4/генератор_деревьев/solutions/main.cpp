#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace std;
using namespace __gnu_pbds;

typedef tree<int, null_type, less<>, rb_tree_tag, tree_order_statistics_node_update> ordered_set;

struct MergeSortTree {
    int n = 1;
    vector<ordered_set> tree;
    vector<int> a;

    MergeSortTree() = default;

    MergeSortTree(int n_, vector<int> a_) {
        while (n <= n_) n *= 2;
        tree.resize(2 * n);
        a = a_;
    }

    void activate(int v, int l, int r, int i) {
        tree[v].insert(a[i]);
        if (l + 1 == r) return;
        int mid = (l + r) / 2;
        if (i < mid) {
            activate(2 * v + 1, l, mid, i);
        } else {
            activate(2 * v + 2, mid, r, i);
        }
    }

    void activate(int i) {
        activate(0, 0, n, i);
    }

    void deactivate(int v, int l, int r, int i) {
        tree[v].erase(a[i]);
        if (l + 1 == r) return;
        int mid = (l + r) / 2;
        if (i < mid) {
            deactivate(2 * v + 1, l, mid, i);
        } else {
            deactivate(2 * v + 2, mid, r, i);
        }
    }

    void deactivate(int i) {
        deactivate(0, 0, n, i);
    }


    int cntless(int v, int l, int r, int lq, int rq, int x) {
        if (l >= rq || lq >= r) return 0;
        if (lq <= l && r <= rq) {
            return tree[v].order_of_key(x);
        }
        int mid = (l + r) / 2;
        return cntless(2 * v + 1, l, mid, lq, rq, x) + cntless(2 * v + 2, mid, r, lq, rq, x);
    }

    int cntless(int lq, int rq, int x) {
        return cntless(0, 0, n, lq, rq, x);
    }

};

struct SegTree {
    int n = 1;
    vector<int> tree;

    SegTree(int n_) {
        while (n <= n_) n *= 2;
        tree.resize(2 * n);
    }

    int get(int v, int l, int r, int lq, int rq) {
        if (lq >= r || l >= rq) return 0;
        if (lq <= l && r <= rq) return tree[v];
        int mid = (l + r) / 2;
        return get(2 * v + 1, l, mid, lq, rq) + get(2 * v + 2, mid, r, lq, rq);
    }

    int get(int lq, int rq) {
        return get(0, 0, n, lq, rq);
    }

    void upd(int v, int l, int r, int i, int x) {
        if (l + 1 == r) {
            tree[v] = x;
            return;
        }
        int mid = (l + r) / 2;
        if (i < mid) {
            upd(2 * v + 1, l, mid, i, x);
        } else {
            upd(2 * v + 2, mid, r, i, x);
        }
        tree[v] = tree[2 * v + 1] + tree[2 * v + 2];
    }

    void upd(int i, int x) {
        upd(0, 0, n, i, x);
    }
};

vector<int> get_fun(int n, int q, vector<int> &a, vector<array<int, 2>> &queries, int MODE) {
    vector<vector<int>> queries_by_r(n);
    for (int i = 0; i < q; i++) {
        queries_by_r[queries[i][1]].push_back(i);
    }
    vector<int> previous_occ(2 * n + 1, -1);
    vector<int> ans(q);
    SegTree st(n);
    for (int i = 0; i < n; i++) {
        if (previous_occ[a[i]] != -1) {
            st.upd(previous_occ[a[i]], MODE);
        }
        previous_occ[a[i]] = i;
        st.upd(i, 1);
        for (auto qi: queries_by_r[i]) {
            int l = queries[qi][0];
            ans[qi] = st.get(l, i + 1);
        }
    }
    return ans;
}

vector<int> get_fun_wx(int n, int q, vector<int> &a, vector<array<int, 2>> &queries, vector<int> &bounds) {
    vector<vector<int>> queries_by_r(n);
    for (int i = 0; i < q; i++) {
        queries_by_r[queries[i][1]].push_back(i);
    }
    vector<int> previous_occ(2 * n + 1, -1);
    vector<int> ans(q);
    MergeSortTree st(n, a);
    for (int i = 0; i < n; i++) {
        if (a[i] > 1) {
            if (previous_occ[a[i]] != -1) {
                st.deactivate(previous_occ[a[i]]);
            }
            previous_occ[a[i]] = i;
            st.activate(i);
        }
        for (auto qi: queries_by_r[i]) {
            int l = queries[qi][0];
            ans[qi] = st.cntless(l, i + 1, bounds[qi] + 1);
        }
    }
    return ans;
}

void solve() {
    int n, q;
    cin >> n >> q;
    vector<int> a(n);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }

    vector<vector<int>> occs(2 * n + 1);
    for (int i = 0; i < n; i++) {
        occs[a[i]].push_back(i);
    }

    auto cnt_on_seg = [&](int l, int r, int x) -> int {
        if (occs[x].empty())return 0;
        if (occs[x][0] > r || occs[x].back() < l) return 0;
        return (int) (upper_bound(occs[x].begin(), occs[x].end(), r) - lower_bound(occs[x].begin(), occs[x].end(), l));
    };

    vector<array<int, 2>> queries(q);
    for (int i = 0; i < q; i++) {
        int l, r;
        cin >> l >> r;
        l--;
        r--;
        queries[i] = {l, r};
    }
    vector<int> cnt_unique = get_fun(n, q, a, queries, 0);
    vector<int> cnt_one_occ = get_fun(n, q, a, queries, -1);
    vector<int> bounds(q);
    for (int i = 0; i < q; i++) {
        int unq = cnt_unique[i], ones = cnt_one_occ[i];
        auto [l, r] = queries[i];
        int len = r - l + 1;
        auto isok = [&](int x) -> bool {
            if (x == 1)return true;
            if (unq >= x + (cnt_on_seg(l, r, x) == 1)) {
                if (unq - ones >= x)return true;
                int s = len - ones + x - (unq - ones);
                if (s > 2 * x - 2)return true;
                if (s == 2 * x - 2 && cnt_on_seg(l, r, x) == 1 && len >= 2 * x - 1)return true;
            }
            return false;
        };
        int L = 1, R = unq + 1;
        while (R - L > 1) {
            int MID = (L + R) / 2;
            if (isok(MID)) {
                L = MID;
            } else {
                R = MID;
            }
        }
        bounds[i] = L;
    }
    vector<int> ans = get_fun_wx(n, q, a, queries, bounds);
    for (int i = 0; i < q; i++) {
        cout << ans[i] << '\n';
    }
}

signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
}