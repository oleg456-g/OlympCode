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
int binup[maxn][K], p[maxn], t[maxn], c[maxn], tin[maxn], tout[maxn], h[maxn], timer = 0;

void dfs(int v) {
    binup[v][0] = p[v];
    h[v] = h[p[v]] + 1;
    tin[v] = timer++;
    for (int i = 1; i < K; i++) {
        binup[v][i] = binup[binup[v][i - 1]][i - 1];
    }
    for (int i: g[v]) {
        dfs(i);
    }
    tout[v] = timer++;
}

bool upper(int a, int b) {
    return tin[a] <= tin[b] && tout[b] <= tout[a];
}

int lca(int a, int b) {
    if (upper(a, b)) return a;
    if (upper(b, a)) return b;
    int cur = a;
    for (int i = K - 1; i >= 0; i--) {
        if (!upper(binup[cur][i], b))
            cur = binup[cur][i];
    }
    return binup[cur][0];
}

vector<pair<int, int>> build_compressed_tree(vi ver) {
    sort(all(ver), [&](int a, int b) {
        return tin[a] < tin[b]; // bug 3
    });
    set<int> was(ver.begin(), ver.end());
    int n = ver.size();
    for (int i = 0; i + 1 < n; i++) {
        int l = lca(ver[i], ver[i + 1]);
        ver.push_back(l);
    }
//    sort(all(ver), [&](int a, int b) {
//        return tout[a] < tout[b]; // bug 2
//    });
    ver.resize(unique(all(ver)) - ver.begin()); // bug 1
    vector<pair<int, int>> ans(ver.size());
    stack<int> segs;
    for (int i = 0; i < ver.size(); i++) {
        ans[i] = {ver[i], (was.find(ver[i]) != was.end() ? 1 : 0)};
        while (!segs.empty() && tin[ver[i]] <= tin[ver[segs.top()]]) {
            ans[i].second += ans[segs.top()].second;
            segs.pop();
        }
        segs.push(i);
    }
    return ans;
}

struct query {
    int time, type, num;
};
list<query> queries[maxn];

struct state {
    int l, r, type;
};
list<state> states[maxn];

vector<ar<int, 3>> changes; // {v, type, old}
map<int, int> segment_tree[maxn * 3];

void update(int v, int l, int r, int ql, int qr, int t, int k_new) {
    if (ql > r || l > qr) {
        return;
    } else if (ql <= l && r <= qr) {
        if (segment_tree[v].find(t) != segment_tree[v].end() && segment_tree[v][t] > k_new)
            return;
        changes.push_back({v, t, segment_tree[v][t]});
        segment_tree[v][t] = k_new;
    } else {
        int mid = (l + r) / 2;
        update(v * 2, l, mid, ql, qr, t, k_new);
        update(v * 2 + 1, mid + 1, r, ql, qr, t, k_new);
    }
}

int get(int v, int l, int r, int pos, int t) {
    if (l == r) {
        return (segment_tree[v].find(t) != segment_tree[v].end() ? segment_tree[v][t] : 0);
    } else {
        int ans = (segment_tree[v].find(t) != segment_tree[v].end() ? segment_tree[v][t] : 0);
        int mid = (l + r) / 2;
        if (pos <= mid) {
            ans = max(ans, get(v * 2, l, mid, pos, t));
        } else {
            ans = max(ans, get(v * 2 + 1, mid + 1, r, pos, t));
        }
        return ans;
    }
}

long long answer[maxn];

int kek = 0;

void dfs_answer(int v) {
    int was = changes.size();
    for (const state &st: states[v]) {
        update(1, 0, maxn, st.l, st.r, st.type, c[v]);
    }
    kek = max(kek, (int)changes.size());
    for (const query &qr: queries[v]) {
        answer[qr.time] = max(answer[qr.time], qr.num * 1ll * get(1, 0, maxn, qr.time, qr.type));
    }
    for (int i: g[v]) {
        dfs_answer(i);
    }
    while (changes.size() > was) {
        segment_tree[changes.back()[0]][changes.back()[1]] = changes.back()[2];
        changes.pop_back();
    }
}

void solve() {
    int n, q;
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
    dfs(0);
    vi last(n);
    vi qq;
    for (int i = 1; i <= q; i++) {
        int type;
        cin >> type;
        if (type == 1) {
            int v, nw;
            cin >> v >> nw;
            v--;
            states[v].push_back({last[v], i - 1, t[v]});
            t[v] = nw;
            last[v] = i;
        } else {
            qq.push_back(i);
            int t, k;
            cin >> t >> k;
            vi a(k);
            cin >> a;
            for (int &j: a)
                j--;
            vpi interest = build_compressed_tree(a);
            for (const pi &j: interest) {
                queries[j.first].push_back({i, t, j.second});
            }
        }
    }
    for (int i = 0; i < n; i++) {
        states[i].push_back({last[i], q, t[i]});
    }
    dfs_answer(0);
    for (int i : qq)
        cout << answer[i] << '\n';
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
