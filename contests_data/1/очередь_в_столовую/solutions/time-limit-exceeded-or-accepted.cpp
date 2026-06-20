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
    }

    int ans = n + 1;
    for (int i = 1; i <= n; i++) {
        long long sum = d;
        for (int j = 1; j < i; j++) {
            sum += k[j];
        }

        bool f = true;
        for (int j = i; j <= n; j++) {
            if (sum > t[j]) {
                f = 0;
                break;
            } else {
                sum += k[j];
            }
        }
        if (f) {
            ans = i;
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