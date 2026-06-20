#include <bits/stdc++.h>

using namespace std;
mt19937_64 rnd(chrono::steady_clock::now().time_since_epoch().count());
#define int long long
#define sz(a) (int)a.size()
#define all(a)a.begin(),a.end()

const int P = 5252525252525252;
const int P2 = 13371337133713371;
const int MAXA = 1e5 + 1;
int lp[MAXA + 1];

void solve() {
    int n;
    cin >> n;
    vector<int> a(n);
    for (auto &x: a) {
        cin >> x;
    }

    set<pair<int,int>> used;
    int ans = 1;
    pair<int,int> hash(0,0);
    used.insert(make_pair(0ll,0ll));
    for (int i = 0; i < n; ++i) {
        pair<int,int> cur = {0,0};
        int x = a[i];
        while (x > 1) {
            int p = lp[x];
            int cnt = 1;
            while (lp[x /= p] == p) {
                cnt ^= 1;
            }
            cur.first ^= p * P * cnt;
            cur.second ^= p * P2 * cnt;
        }
        pair<int,int> rem = hash;
        hash.first ^= cur.first;
        hash.second ^= cur.second;
        if (used.contains(hash)) {
            ++ans;
            used.clear();
            used.insert(rem);
        }
        used.insert(hash);
    }
    cout << ans << "\n";
}

signed main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    //http://e-maxx.ru/algo/prime_sieve_linear
    vector<int> pr;

    for (int i = 2; i <= MAXA; ++i) {
        if (lp[i] == 0) {
            lp[i] = i;
            pr.push_back(i);
        }
        for (int j = 0; j < (int) pr.size() && pr[j] <= lp[i] && i * pr[j] <= MAXA; ++j)
            lp[i * pr[j]] = pr[j];
    }

    solve();
    return 0;
}
