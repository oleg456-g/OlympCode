#include <bits/stdc++.h>

#define all(x) (x).begin(), (x).end()
#define len(x) (int) (x).size()
#define endl "\n"
#define int __int128_t
#define ll long long

using namespace std;


signed main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ll n, q;
    cin >> n >> q;
    vector <int> a(n), b(n);
    for (int& i : a) {
        ll x;
        cin >> x;
        i = x;
    }
    for (int& i : b) {
        ll x;
        cin >> x;
        i = x;
    }
    vector <int> req(q);
    for (int& i : req) {
        ll x;
        cin >> x;
        i = x;
    }

    vector <pair <int, int>> bord(q, {-1LL, LLONG_MAX / 8});
    while (true) {
        vector <array <int, 2>> events;
        for (int i = 0; i < q; i++) {
            if (bord[i].first + 1 != bord[i].second) {
                events.push_back({(bord[i].first + bord[i].second) / 2, i});
            }
        }
        if (events.empty()) {
            break;
        }

        map <int, vector <pair <int, int>>> end_here;
        for (int i = 0; i < n; i++) {
            if (b[i] != 0) {
                events.push_back({a[i] / b[i], INT_MAX});
                end_here[a[i] / b[i]].emplace_back(b[i] - a[i] % b[i], b[i]);
            }
        }
        sort(all(events));
        events.erase(unique(all(events)), events.end());

        int m = 0;
        int now_bou = 0;
        int need_for_one = 0;
        for (auto [cnt, type] : events) {
            if (type == INT_MAX) {
                m += (cnt - now_bou + 1) * need_for_one;
                now_bou = cnt + 1;
                for (auto i : end_here[cnt]) {
                    m += i.first;
                    need_for_one += i.second;
                }
            } else {
                m += (cnt - now_bou) * need_for_one;
                now_bou = cnt;
                if (m <= req[type]) {
                    bord[type].first = cnt;
                } else {
                    bord[type].second = cnt;
                }
            }
        }
    }
    for (auto [ans, _] : bord) {
        cout << (ll) ans << " ";
    }
    cout << endl;
    return 0;
}
