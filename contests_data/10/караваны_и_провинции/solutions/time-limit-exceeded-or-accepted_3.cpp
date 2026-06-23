#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <tuple>
#include <math.h>
#include <set>
#include <stack>
#include <bitset>
#include <map>
#include <queue>
#include <random>
#include <array>
#include <unordered_set>
#include <cmath>
#include <cassert>
#include <unordered_map>
#include <time.h>
#include <list>
#include <complex>
#include <thread>

#define rep(i, n) for (int i = 0; i < (n); ++i)
#define rep1(i, n) for (int i = 1; i < (n); ++i)
#define rep1n(i, n) for (int i = 1; i <= (n); ++i)
#define repr(i, n) for (int i = (n) - 1; i >= 0; --i)
#define pb push_back
#define eb emplace_back
#define all(a) (a).begin(), (a).end()
#define rall(a) (a).rbegin(), (a).rend()
#define each(x, a) for (auto &x : a)
#define ar array
//#define vec vector
#define range(i, n) rep(i, n)
// #define int long long

using namespace std;

using ll = long long;
using ull = unsigned long long;
using ld = double;
using str = string;
using pi = pair<int, int>;
using pl = pair<ll, ll>;

using vi = vector<int>;
using vu = vector<unsigned int>;
using vl = vector<ll>;
using vull = vector<unsigned long long>;
using vpi = vector<pair<int, int>>;
using vvi = vector<vi>;

int Bit(int mask, int b) { return (mask >> b) & 1; }

template<class T>
bool ckmin(T &a, const T &b) {
    if (b < a) {
        a = b;
        return true;
    }
    return false;
}

template<class T>
bool ckmax(T &a, const T &b) {
    if (b > a) {
        a = b;
        return true;
    }
    return false;
}

const int INFi = 1e9 + 228;
const ll INF = 1e18;
const int LG = 20;
const int N = 1 << LG;
const ld eps = 1e-7;

const int mod = 1e9 + 7;

template<typename T>
std::istream &operator>>(std::istream &in, vector<T> &a) {
    for (T &i: a) {
        in >> i;
    }
    return in;
}

const int maxn = 2e5 + 228, K = 18;

vi g[maxn];
int p[maxn], sz[maxn], t[maxn], c[maxn];
int query_t;
long long ans = 0;

void dfs(int v) {
    for (int i : g[v]) {
        dfs(i);
        sz[v] += sz[i];
    }
    if (t[v] == query_t) {
        ans = max(ans, sz[v] * 1ll * c[v]);
    }
}

void solve() {
    int n, q, _;
    cin >> n >> q;
    for (int i = 1; i < n; i++) {
        cin >> p[i];
        p[i]--;
        g[p[i]].push_back(i);
    }
    for (int i = 0; i < n; i++) {
        cin >> t[i];
    }
    for (int i = 0; i < n; i++) {
        cin >> c[i];
    }
    for (int i = 0; i < q; i++) {
        int type;
        cin >> type;
        if (type == 1) {
            int v, t_new;
            cin >> v >> t_new;
            v--;
            t[v] = t_new;
        } else {
            for (int i = 0; i < n; i++)
                sz[i] = 0;
            int k;
            cin >> query_t >> k;
            for (int i = 0; i < k; i++) {
                int a;
                cin >> a;
                sz[a - 1]++;
            }
            ans = 0;
            dfs(0);
            cout << ans << endl;
        }
    }

}

signed main() {
    srand(time(NULL));
    ios_base::sync_with_stdio(false);
    cin.tie(0);
//    cout << setprecision(15) << fixed;
    int q = 1;
//    cin >> q;
    while (q--)
        solve();
    return 0;
}
