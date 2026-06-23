#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using ld = long double;

#define all(a) begin(a), end(a)
#define len(a) int((a).size())

#ifdef LOCAL
    #include "debug.h"
#else
    #define dbg(...)
    #define dprint(...)
    #define debug if constexpr (false)
    #define draw_tree(...)
    #define draw_array(...)
#endif // LOCAL

int main() {
    cin.tie(nullptr)->sync_with_stdio(false);

    int n, q;
    cin >> n >> q;
    vector<int> a(n);
    for (auto &x : a) {
        cin >> x;
    }
    vector<int> b(n);
    for (auto &x : b) {
        x = 1;
    }

    struct data {
        int have;
        int a;
        int b;
    };

    vector<data> opts;
    for (int i = 0; i < n; i++) {
        if (b[i] == 0) {
            continue;
        }
        opts.push_back({(a[i] + b[i] - 1) / b[i], a[i], b[i]});
    }
    sort(opts.begin(), opts.end(), [&](auto a, auto b) {
        return a.have < b.have;
    });

    vector<ll> pref_a(len(opts) + 1);
    vector<ll> pref_b(len(opts) + 1);
    for (int i = 0; i < len(opts); i++) {
        pref_a[i + 1] = pref_a[i] + opts[i].a;
        pref_b[i + 1] = pref_b[i] + opts[i].b;
    }
    
    vector<int> prev(len(opts), -1);
    for (int i = 1; i < len(opts); i++) {
        if (opts[i - 1].have == opts[i].have) {
            prev[i] = prev[i - 1];
        } else {
            prev[i] = i - 1;
        }
    }

    while (q--) {
        ll x;
        cin >> x;

        int left = -1;
        int right = len(opts);
        while (right - left > 1) {
            int mid = (left + right) / 2;
            (pref_b[mid + 1] * opts[mid].have - pref_a[mid + 1] > x ? right : left) = mid;
        }
        if (right < len(opts)) {
            right = prev[right] + 1;
        }

        if (right == 0) {
            cout << "0\n";
        } else {
            ll start_value = opts[right - 1].have;
            x -= pref_b[right] * start_value - pref_a[right];
            ll ans = start_value + x / pref_b[right];
            if (right < len(opts)) {
                ans = min<ll>(ans, opts[right].have - 1);
            }
            cout << ans << '\n';
        }
    }
}
