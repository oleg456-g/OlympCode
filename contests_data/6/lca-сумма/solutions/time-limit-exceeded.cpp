#include <bits/stdc++.h>

using namespace std;
using ll = long long;

constexpr int N = 100'000;
constexpr int MOD = 1'000'000'007;

vector<int> go[N+1];
vector<int> sons;

int ans = 0;

// Возвращает 2^sz(v). Делает прибавления к ans
int dfs(int v) {
    int c = 1;

    vector<int> sons;
    for (int to : go[v]) {
        int x = dfs(to);
        sons.push_back(x);
        c = (ll) c * x % MOD;
    }

    int was = ans;
    ans = (ans + (ll)c*v)%MOD;

    // переберем первые два дерева, в которых есть вершина
    int acc = v;
    for (int i = 0; i+1 < sons.size(); ++i) {
        int suf = 0;
        for (int j = i+1; j < sons.size(); ++j) {
            suf += sons[j]-1;
            if (suf >= MOD) suf -= MOD;
        }
        ans = (ans + (ll)suf*acc%MOD*(sons[i]-1))%MOD;
        acc = (ll)acc*sons[i]%MOD;
    }

    return 2*c%MOD;
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