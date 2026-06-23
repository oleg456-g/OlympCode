#include <bits/stdc++.h>

using namespace std;
using ll = long long;

constexpr int N = 100'000;
constexpr int MOD = 1'000'000'007;

vector<int> go[N+1];

int ans = 0;

int dfs(int v) {
    int c = 2, s = 1;
    for (int to : go[v]) {
        int x = dfs(to);
        s += x-1; if (s >= MOD) s -= MOD;
        c = (ll) c * x % MOD;
    }
    ans = (ans + (ll)(c-s)*v)%MOD;
    if (ans < 0) ans += MOD;
    return c;
}

void solve() {
    int n; cin >> n;
    for (int i = 1; i <= n; ++i) go[i].clear();
    
    int v = 0;
    for (int i = 1; i <= n; ++i) {
        int p; cin >> p;
        if (p != i) go[p].push_back(i);
        else v = p;
    }

    ans = 0; 
    dfs(v);
    cout << ans << '\n';   
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0);

    int t; cin >> t;
    while (t--) solve();
}