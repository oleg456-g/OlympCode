#include <bits/stdc++.h>

#define int long long
using namespace std;
const int maxx = 1e18;

signed main() {
    int n, q;
    cin >> n >> q;
    int a[n];
    int b[n];
    int x[q];
    int div[n];
    int need[n];
    int pref_b[n + 1];
    int answers[q];
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }
    vector<int> id;
    for (int i = 0; i < n; ++i) {
        cin >> b[i];
        if (b[i] == 0) {
            continue;
        }
        id.push_back(i);
        div[i] = a[i] / b[i];
        need[i] = (div[i] + 1) * b[i] - a[i];
    }
    for (int i = 0; i < q; ++i) {
        cin >> x[i];
    }
    sort(id.begin(), id.end(), [&div](int id1, int id2) -> bool {
        return div[id1] < div[id2];
    });
    int ans = 0;
    int now = 0;
    vector<pair<int, pair<int, int>>> mp;
    pref_b[0] = 0;
    for (int i = 0; i < id.size(); ++i) {
        int ind = id[i];
        int cnt = div[ind] + 1 - now;
        ans += pref_b[i] * cnt + need[ind];
        now = div[ind] + 1;
        pref_b[i + 1] = pref_b[i] + b[ind];
        if (i == id.size() - 1 || div[id[i]] != div[id[i + 1]]) {
            mp.push_back({ans, {i, div[ind] + 1}});
        }
    }
    for (int i = 0; i < q; ++i) {
        auto it = upper_bound(mp.begin(), mp.end(), make_pair(x[i] + 1, make_pair(-1ll, 0ll)));
        int limit = 2e18;
        if (it != mp.end()) {
            limit = it->second.second;
        }
        if (it == mp.begin()) {
            answers[i] = limit - 1;
            continue;
        }
        it--;
        int o = it->second.second;
        int ind = it->second.first;
        int left = x[i] - it->first;
        o += left / pref_b[ind + 1];
        answers[i] = min(o, limit - 1);
    }
    for (int i = 0; i < q; ++i) {
        cout << answers[i] << " ";
    }
}
