#include <bits/stdc++.h>
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

const int MAXN = 300010;

string s[MAXN];
ll dist[MAXN];
bool used[MAXN];

void solve() {
    int n, k;
    cin >> n >> k;

    fill(s, s + n, "");
    fill(dist, dist + n, ll(1e12));
    fill(used, used + n, false);

    dist[0] = 0;

    for (int i = 0; i < n; i++)
        cin >> s[i];
    for (int _ = 0; _ < n; _++) {
        int mn = -1;
        for (int i = 0; i < n; i++)
            if (!used[i] && (mn == -1 || dist[i] < dist[mn]))
                mn = i;
        used[mn] = true;
        for (int i = 0; i < n; i++) {
            int sz = min(s[mn].size(), s[i].size()) + 1, gsz = -1;
            bool last = false;
            while (sz > k) {
                sz--;
                bool flag = true;
                for (int j = 0; j < sz; j++)
                    if (s[mn][s[mn].size() - sz + j] != s[i][j]) {
                        flag = false;
                        break;
                    }
                if (flag && !last)
                    gsz = sz;
                last = flag;
            }
            if (gsz >= k && dist[mn] + gsz < dist[i])
                dist[i] = dist[mn] + gsz;
        }
    }

    for (int i = 1; i < n; i++)
        cout << (dist[i] == ll(1e12) ? -1 : dist[i]) << ' ';
    cout << endl;
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
