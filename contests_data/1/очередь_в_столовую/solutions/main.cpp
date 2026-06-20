#include <iostream>
#include <vector>

using namespace std;

void solve() {
    int n, d;
    cin >> n >> d;
    vector<int> t(n + 1), k(n + 1);
    vector<long long> pref(n + 1);
    for (int i = 1; i < n + 1; i++) {
        cin >> t[i] >> k[i];
        pref[i] = pref[i - 1] + k[i - 1];
    }

    int ans = n + 1;
    for (int i = n; i > 0; i--) {
        if (pref[i] + d <= t[i]) {
            ans = i;
        } else {
            break;
        }
    }

    cout << ans << "\n";
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    cin >> t;
    while (t--) {
        solve();
    }

    return 0;
}