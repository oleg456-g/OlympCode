#include <algorithm>
#include <bits/stdc++.h>
#include <queue>

using namespace std;
using ll = long long;
using pi = pair<int, int>;

template<typename T>
bool ckmin(T& a, const T& b) { return b < a ? a = b, true : false; }
template<typename T>
bool ckmax(T& a, const T& b) { return a < b ? a = b, true : false; }

template<typename T>
using pq_min = priority_queue<T, vector<T>, greater<T>>;
template<typename T>
using pq_max = priority_queue<T, vector<T>, less<T>>;

struct Fenwick {
    int n;
    vector<int> f;

    void Init(int n0) {
        n = n0;
        f.assign(n, 0);
    }

    void Add(int i, int x) {
        for (; i < n; i |= i + 1)
            f[i] += x;
    }

    int Sum(int j) {
        int res = 0;
        for (--j; j >= 0; j = (j&(j+1))-1)
            res += f[j];
        return res;
    }
};

struct MergeSort {
    int n, sz, h;
    vector<vector<int>> values;
    vector<Fenwick> layers;

    MergeSort(int n0, const vector<int>& a) {
        n = n0;
        sz = 1, h = 0;
        while (sz < n) sz += sz, ++h;

        values.resize(h+1);
        layers.resize(h+1);
        for (int u = 0; u <= h; ++u) {
            const int len = 1 << (h - u);
            values[u] = a;
            layers[u].Init(n);

            for (int l = 0; l < n; l += len) {
                const int r = min(n, l + len);
                sort(values[u].begin()+l, values[u].begin()+r);
            }
        }
    }

    void Add(int i, int x) {
        // cout << "Add " << i << " " << x << endl;
        for (int u = 0; u <= h; ++u) {
            const int len = 1 << (h-u);
            const int l = i & ~(len-1);
            const int r = min(n, l+len);
            auto it = lower_bound(values[u].begin()+l, values[u].begin()+r, values[h][i]);
            const int j = it - values[u].begin();
            // if (i == 1 && u == 2) cout << "UPD!!! " << j << " " << u << " +=" << x << endl;
            layers[u].Add(j, x);
        }
    }

    int SumDetail(int u, int l, int x) {
        const int r = min(n, l + (1 << (h-u)));
        auto it = lower_bound(values[u].begin()+l, values[u].begin()+r, x);
        const int j = it - values[u].begin();
        int res = layers[u].Sum(j) - layers[u].Sum(l);
        // cout << u << " " << l << ":" << r << " => " << x << ":" << j << " => len=" << (1 << (h-u)) << ", res=" << res << endl;
        return res;
    }

    int SumLess(int l, int r, int x) {
        int res = 0;

        // cout << "SumLess " << l << ":" << r << " => " << x << endl;
        for (int u = h; u >= 0 && l < r; --u) {
            const int len = 1 << (h-u);
            if (l & len) {
                res += SumDetail(u, l, x);
                l += len;
            }
            if (r & len) {
                r -= len;
                res += SumDetail(u, r, x);
            }
        }
        return res;
    }
};

void solve() {
    int n, q; cin >> n >> q;
    vector<int> a(n);
    for (int& x : a) cin >> x;

    vector<pi> w(n);
    for (int i = n; i--; ) w[i] = { a[i], i };
    sort(w.begin(), w.end());

    vector<int> ql(q), qr(q), ord(q), ans(q);
    for (int i = 0; i < q; ++i) cin >> ql[i] >> qr[i], --ql[i], ord[i] = i;
    sort(ord.begin(), ord.end(), [&](int l, int r) { return qr[l] < qr[r]; });

    Fenwick fnw;
    fnw.Init(n);
    MergeSort sgt(n, a);
    vector<int> msk(n);

    vector<int> last(2*n+1, -1);
    vector<int> prv(n, -1);
    int j = 0;
    for (int i : ord) {
        const int l = ql[i], r = qr[i];
        while (j < r) {
            // cout << "see j=" << j << " " << r << endl;
            if (last[a[j]] != -1) {
                fnw.Add(last[a[j]], -1);
                if (a[j] != 1) sgt.Add(last[a[j]], -1);
                if (a[j] != 1) msk[last[a[j]]]--;
            }
            fnw.Add(j, +1);
            if (a[j] != 1) sgt.Add(j, +1);
            prv[j] = last[a[j]];
            last[a[j]] = j;
            if (a[j] != 1) msk[j]++;

            ++j;
        }

        // for (int l = 0; l < n; ++l) {
        //     for (int r = l + 1; r <= n; ++r) {
        //         for (int x = 0; x <= 2*n; ++x) {
        //             int jans = 0;
        //             for (int i = l; i != r; ++i)
        //                 if (a[i] < x) jans += msk[i];
        //             int pans = sgt.SumLess(l, r, x);
        //             if (pans != jans) {
        //                 cout << "j=" << j << endl;
        //                 for (int x : msk) cout << x << ' ';
        //                 cout << endl;
        //                 cout << "seg=" << l << ".." << r << ", x=" << x << endl;
        //                 cout << "pans=" << pans << ", jans=" << jans << endl;
        //                 assert(pans == jans);
        //             }
        //         }
        //     }
        // }

        const int uniq_cnt = fnw.Sum(r) - fnw.Sum(l);
        // for (int x = 2; x <= n; ++x) {
        //     if (last[x] < l) continue;
        //     const int j = last[x];
        //     const int uc = uniq_cnt - (prv[j] < l);
        //     if (x <= min(uc, r-l-uc+1))
        //         ++ans[i];
        // }

        const int maxk = min(uniq_cnt - 1, r - l - uniq_cnt + 1);
        ans[i] = sgt.SumLess(l, r, maxk+1);

        // // if (i == 1) cout << uniq_cnt << " " << r - l - uniq_cnt + 1 << " " << maxk << endl;
        if (uniq_cnt > maxk && last[maxk+1] >= l && maxk >= 1) {
            // if (i == 1) cout << uniq_cnt << " " << maxk << endl;
            const int k = maxk+1;
            const int j = last[k];
            const int j2 = prv[j];

            int uc = j2 >= l ? uniq_cnt : uniq_cnt - 1;
            if (k <= min(uc, r - l - uc + 1))
                ++ans[i];
        }
    }

    for (int x : ans) cout << x << '\n';
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0);
    int t = 1;
    cin >> t;
    while (t--) solve();
}