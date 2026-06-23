#include <bits/stdc++.h>

#pragma GCC optimize("O3,unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")

#define LF '\n'
#define SP ' '
#define all(x) x.begin(), x.end()
#define rep(i, a, b) for (int i = a; i < (b); i++)
using namespace std;
using ll = long long;
using ull = unsigned long long;
using ld = long double;

constexpr int MAXV = 1e9;
constexpr int MAXF = 13;

constexpr int MAXQ = 5000;
vector<int> pts;
int pt_sum[2*MAXQ + 10];

vector<int> gorilla_nums;

void f(ll x = 1, int i = 2, int last_k = 30) {
    if (i == MAXF) {
        gorilla_nums.push_back(x);
        return;
    }

    rep(k, 0, last_k + 1) {
        f(x, i + 1, k);

        x *= i;
        if (x > MAXV) break;
    }
}

// unordered_map<int, int> cnt;

void precalc() {
    f();
    sort(all(gorilla_nums));
    gorilla_nums.erase(unique(all(gorilla_nums)), gorilla_nums.end());
    // cout << gorilla_nums.size() << LF;
    // size is 528

    vector<int> p_table = {6, 3, 1};

    // ll max_v = 0;
    ll v = 0;
    rep(i, 0, gorilla_nums.size()) {
        v += gorilla_nums[i];
        rep(j, i, gorilla_nums.size()) {
            int x = (gorilla_nums[i] == gorilla_nums[j]);

            v += gorilla_nums[j];
            rep(k, j, gorilla_nums.size()) {
                v += gorilla_nums[k];

                if (v > MAXV) {
                    v -= gorilla_nums[k];
                    continue;
                }
                // max_v = max(v, max_v);

                // int x = int(gorilla_nums[i] == gorilla_nums[j]) + int(gorilla_nums[j] == gorilla_nums[k]);
                int p = p_table[x + (gorilla_nums[j] == gorilla_nums[k])];
                int ind = lower_bound(all(pts), v) - pts.begin();
                pt_sum[ind] += p;

                v -= gorilla_nums[k];
            }
            v -= gorilla_nums[j];
        }
        v -= gorilla_nums[i];
    }
    // cout << "max_v: " << max_v << LF;
    // cout << "cnt.size(): " << cnt.size() << LF;

    rep(i, 1, pts.size()) {
        pt_sum[i] += pt_sum[i - 1];
    }
}

void solve() {
    // int n;
    // cin >> n;

}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    int t;
    cin >> t;
    vector<pair<int, int>> q(t);
    rep(i, 0, t) {
        int l, r;
        cin >> l >> r;
        q[i] = {l, r};
        pts.push_back(l - 1);
        pts.push_back(r);
    }
    sort(all(pts));
    pts.erase(unique(all(pts)), pts.end());

    pts.push_back(int(1e9) + 10);

    precalc();

    rep(i, 0, t) {
        auto [l, r] = q[i];
        int ind_r = lower_bound(all(pts), r) - pts.begin();
        int ind_l = lower_bound(all(pts), l - 1) - pts.begin();
        int ans = pt_sum[ind_r] - pt_sum[ind_l];
        cout << ans << LF;
    }

    /*
    precalc();
    return 0;
    cout << endl;
    cout << "waiting for t..." << endl;

    int t;
    cin >> t;
    while (t --> 0) {
        solve();
    }
    */
}
