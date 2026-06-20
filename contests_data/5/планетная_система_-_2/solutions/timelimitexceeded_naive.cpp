#include <bits/stdc++.h>
using namespace std;
const int mod = 1e9 + 7;
void solve() {
    int n, m, k;
    cin >> n >> m >> k;
    vector<vector<int>> masks;
    for (int i = 0; i < m; i++) {
        int a;
        cin >> a;
    }
    for (int i = 0; i < n; i++) {
        vector<int> val(m);
        for (int j = 0; j < m; j++) {
            cin >> val[j];
        }
        masks.push_back(val);
    }
    vector<bool> habitable(n);
    for (int i = 0; i < k; i++) {
        int ind;
        cin >> ind;
        habitable[ind - 1] = true;
    }
    vector<pair<vector<int>, int>> sorted(n);
    for (int i = 0; i < masks.size(); i++) {
        sorted[i] = {masks[i], i};
    }
    sort(sorted.begin(), sorted.end());
    vector<long long> ans(n);
    for (auto [msk, ind] : sorted) {
        ans[ind] = habitable[ind];
        for (int j = 0; j < n; j++) {
            if (ind == j) {
                continue;
            }
            bool ok = true;
            for (int f = 0; f < m; f++) {
                if (masks[ind][f] < masks[j][f]) {
                    ok = false;
                }
            }
            if (ok) {
                ans[ind] = ans[ind] + ans[j];
            }
        }
        ans[ind] %= mod;
    }
    for (int i = 0; i < n; i++) {
        cout << ans[i] << '\n';
    }
}
int main() {
    cin.tie(NULL);
    ios_base::sync_with_stdio(false);
    int t;
    cin >> t;
    //t = 1;
    while (t--) {
        solve();
    }
}