#include <bits/stdc++.h>

using namespace std;

vector<pair<int, int>> ans;

bool naive_sort(int l, int r, int k, vector<int> &a) {
    map<vector<int>, pair<vector<int>, pair<int, int>>> par;

    auto get_vector_seg = [&](int l, int r) -> vector<int>{
        vector<int> z;
        for (int i = l; i < r; ++i) {
            z.push_back(a[i]);
        }
        return z;
    };
    auto get_sorted = [&]() -> vector<int> {
        auto tmp = get_vector_seg(l, r);
        sort(tmp.begin(), tmp.end());
        return tmp;
    };

    auto start = get_vector_seg(l, r);
    // auto _start = a;

    par[start] = {};
    queue<vector<int>> st;
    st.push(start);
    auto sorted = get_sorted();
    while (st.size() && !par.count(sorted)) {
        auto tmp = st.front();
        st.pop();
        for (int i = 0; i + k <= (int) tmp.size(); ++i) {
            auto z = tmp;
            int ind1 = min_element(tmp.begin() + i, tmp.begin() + i + k) - tmp.begin();
            int ind2 = max_element(tmp.begin() + i, tmp.begin() + i + k) - tmp.begin();
            swap(z[ind1], z[ind2]);
            if (!par.count(z)) {
                par[z] = {tmp, {l + i, l + i + k}};
                st.push(z);
            }
        }
    }
    if (par.count(sorted)) {
        for (int i = l; i < r; ++i) {
            a[i] = sorted[i - l];
        }
        vector<pair<int, int>> ops;
        while (sorted != start) {
            auto z = par[sorted];
            ops.push_back(z.second);
            sorted = z.first;
        }
        reverse(ops.begin(), ops.end());
        for (auto i : ops) {
            ans.push_back(i);
        }
        return true;
    } else {
        return false;
    }
}

void solve() {
    int n, k;
    cin >> n >> k;
    vector<int> a(n);
    ans.clear();
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }
    int K = 2 * k;
    vector<int> st = a;
    if (n < K) {
        if (!naive_sort(0, n, k, a)) {
            cout << "NO\n";
            return;
        }
        cout << "YES\n";
        cout << ans.size() << '\n';
        for (auto i : ans) {
            swap(st[i.first], st[i.second - 1]);
            cout << i.first + 1 << ' ' << i.second << '\n';
        }
        return;
    }
    for (int tt = 0; tt < n; ++tt) {
        for (int i = 0; i + K <= n - tt; i += K - 1) {
            assert(naive_sort(i, i + K, k, a));
        }
        if (n - tt - K >= 0)
            assert(naive_sort(n - tt - K, n, k, a));
    }
    cout << "YES\n";
    cout << ans.size() << '\n';
    for (auto i : ans) {
        swap(st[i.first], st[i.second - 1]);
        cout << i.first + 1 << ' ' << i.second << '\n';
    }
    for (int i : st) {
        cerr << i << ' ';
    }
    cerr << '\n';
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    int t;
    cin >> t;
    while (t--)
        solve();
}