#include <bits/stdc++.h>

using namespace std;

const int long long INF = 1e18 + 228;

void solve() {
    int n, k;
    cin >> n >> k;
    vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }
    int ans = 0;
    bool find = false;
    while (!find) {
        ++ans;
        bool meow = false;
        for (int i = 0; !meow && i < n; ++i) {
            if (a[i]) {
                --a[i];
                for (int j = n - 1; !meow && j >= 0; --j) {
                    if (a[j]) {
                        --a[j];
                        meow = true;
                        if (j - i <= k) {
                            find = true;
                            break;
                        }
                    }
                }
            }
        }
        if (!meow) {
            break;
        }
    }
    cout << ans << '\n';
}

int main(int argc, char* argv[]) {
    int t;
    cin >> t;
    while (t--)
        solve();
}
