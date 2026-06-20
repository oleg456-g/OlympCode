#include <bits/stdc++.h>
#pragma GCC optimize("O3")
using namespace std;
typedef long long ll;
typedef long double ld;
typedef unsigned long long ull;
 
#define F first
#define S second
#define endl '\n'
 
mt19937 rng(chrono::high_resolution_clock::now().time_since_epoch().count());
 
uniform_int_distribution<int> uid(0, 10000000);
 
int rnd(int n) {
    return uid(rng) % n;
}
 
double rndd() { return double(rand()) / RAND_MAX; }
 
int p[100010];
 
void solve() {
    int n, q;
    cin >> n >> q;
    for (int i = 0; i < n; i++)
        cin >> p[i];
    while (q--) {
        int l, r;
        cin >> l >> r;
        l--, r--;
        unordered_map <int, int> mp;
        mp.reserve(r - l + 2);
        for (int i = l; i <= r; i++)
            mp[p[i]]++;
        int cnt = 0;
        for (int k = 2; k <= mp.size(); k++) {
            if (mp[k] == 0)
                continue;
            priority_queue <pair <int, int>> sv;
            for (auto& el : mp)
                if (el.S - (el.F == k) > 0)
                    sv.push({el.S - (el.F == k), el.F});
            if (k > sv.size())
                continue;
            int sumk = 0;
            for (int i = 0; i < k; i++) {
                sumk += sv.top().F;
                sv.pop();
            }
            if ((mp[k] > 1 && mp.size() >= k) || (mp[k] == 1 && mp.size() >= k + 1)) {
                if (sumk >= 2 * k - 2)
                    cnt++;
                else
                    break;
            }
            else
                break;
        }
        cout << cnt << endl;
    }
}
 
signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
 
    auto start_time = chrono::high_resolution_clock::now();
 
    int tt = 1;
    cin >> tt;
    for (int _ = 0; _ < tt; _++)
        solve();
 
    auto end_time = chrono::high_resolution_clock::now();
    cerr << "Execution time: " <<
            chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << " ms" << endl;
}
