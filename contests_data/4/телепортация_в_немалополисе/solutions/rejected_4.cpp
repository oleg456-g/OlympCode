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

vector<pair<int, int> > g[MAXN];

pair<string, int> s[MAXN];
int dist[MAXN];

void solve() {
    int n, k;
    cin >> n >> k;

    fill(s, s + n, pair<string, int>{"", 0});
    fill(g, g + n, vector<pair<int, int> >{});
    fill(dist, dist + n, int(1e9));

    dist[0] = 0;

    for (int i = 0; i < n; i++) {
        cin >> s[i].F;
        s[i].S = i;
    }
    sort(s, s + n);

    for (int i = 0; i < n; i++) {
        bool flagg = true;
        for (int j = s[i].F.size() - 1; j > s[i].F.size() - k; j--)
            flagg &= s[i].F[j] == s[i].F.back();
        for (int j = s[i].F.size() - k; j >= max(0, (int)s[i].F.size() - 20); j--) {
            flagg &= s[i].F[j] == s[i].F.back();
            int sz = s[i].F.size() - j;
            int l = -1, r = n;
            while (l + 1 < r) {
                int m = (l + r) / 2;
                bool flag = true;
                for (int ind = 0; ind < sz; ind++) {
                    if (s[i].F[j + ind] != s[m].F[ind]) {
                        if (s[i].F[j + ind] > s[m].F[ind])
                            l = m;
                        else
                            r = m;
                        flag = false;
                        break;
                    }
                }
                if (flag)
                    r = m;
            }
            if (s[i].F.substr(j, sz) != s[r].F.substr(0, sz))
                continue;
            int c1 = r;
            l = c1 - 1, r = n;
            while (l + 1 < r) {
                int m = (l + r) / 2;
                bool flag = true;
                for (int ind = 0; ind < sz; ind++) {
                    if (s[i].F[j + ind] != s[m].F[ind]) {
                        if (s[i].F[j + ind] > s[m].F[ind])
                            l = m;
                        else
                            r = m;
                        flag = false;
                        break;
                    }
                }
                if (flag)
                    l = m;
            }
            int c2 = l;
            if (c1 <= c2)
                for (int ind = c1; ind <= c2; ind++)
                    if (s[i].S != s[ind].S && (!flagg || (sz >= s[i].F.size() ? '#' : s[i].F[j - 1]) != s[i].F[j] || (sz >= s[i].F.size() ? '#' : s[i].F[j - 1]) != (sz >= s[ind].F.size() ? '$' : s[ind].F[sz])))
                        g[s[i].S].push_back({s[ind].S, sz});
        }
    }

    set<pair<int, int> > pq;
    pq.insert({0, 0});

    while (!pq.empty()) {
        int ind = pq.begin()->S;
        pq.erase(pq.begin());
        for (auto &u: g[ind])
            if (dist[u.F] > dist[ind] + u.S) {
                pq.erase({-dist[u.F], u.F});
                dist[u.F] = dist[ind] + u.S;
                pq.insert({-dist[u.F], u.F});
            }
    }

    for (int i = 1; i < n; i++)
        cout << (dist[i] == int(1e9) ? -1 : dist[i]) << ' ';
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
