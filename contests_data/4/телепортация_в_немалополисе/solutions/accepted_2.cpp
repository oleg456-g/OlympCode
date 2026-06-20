#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef long double ld;
typedef unsigned long long ull;

#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;

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
const ll INF = 1e15;

string s[MAXN];

struct node {
    bool flag;
    int nxt[26], l, r, cnt;
    vector<int> tm;

    node() {
        fill(nxt, nxt + 26, -1);
        l = -1;
        r = -1;
        flag = true;
        cnt = 0;
    }
};

int szb, start_do;
vector<node> b;

void b_add(int num, string &s, int ib = 0, int is = 0, char last = 0) {
    if (is >= s.size()) {
        b[ib].tm.push_back(num);
        b[ib].cnt++;
        return;
    }
    if (b[ib].nxt[s[is] - 'a'] == -1) {
        b[ib].nxt[s[is] - 'a'] = szb;
        b.push_back({});
        b.back().flag = b[ib].flag & (last == 0 || last == s[is]);
        szb++;
    }
    b_add(num, s, b[ib].nxt[s[is] - 'a'], is + 1, s[is]);
}

struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return (hash1 << 5) ^ hash2;
    }
};

const ll MOD1 = 1000000021, MOD2 = 1000000033;
const ll P1 = 73, P2 = 73;
gp_hash_table<pair <ll, ll>, int, hash_pair> mp;
int ido_to_ib[MAXN * 2];

void b_get_sorted(int ib = 0, pair<ll, ll> h = {0, 0}) {
    mp[h] = ib;
    for (int i = 0; i < 26; i++)
        if (b[ib].nxt[i] != -1) {
            b_get_sorted(b[ib].nxt[i], {
                             (h.F * P1 + (i + 1)) % MOD1,
                             (h.S * P2 + (i + 1)) % MOD2
                         });
            for (auto &el: b[b[ib].nxt[i]].tm)
                b[ib].tm.push_back(el);
        }
    b[ib].l = start_do;
    b[ib].r = start_do + b[ib].tm.size() - 1;
    start_do += b[ib].tm.size();
    for (int j = b[ib].l; j <= b[ib].r; j++)
        ido_to_ib[j] = ib;
}

const int MAXR = 1 << 20;

int R;
pair <ll, ll> tr[MAXR << 1];
ll ph[MAXR << 1];
ll dist[MAXN];

void build() {
    for (int i = 0; i < R; i++)
        tr[i + R] = {INF, i};
    for (int i = R - 1; i > 0; i--)
        tr[i] = tr[i * 2];
    fill(ph, ph + 2 * R, -1);
}

void push(int node) {
    if (ph[node] == -1)
        return;
    if (tr[node].S != INF)
        tr[node].F = min(tr[node].F, ph[node]);
    if (node < R) {
        if (ph[node * 2] != -1)
            ph[node * 2] = min(ph[node * 2], ph[node]);
        else
            ph[node * 2] = ph[node];
        if (ph[node * 2 + 1] != -1)
            ph[node * 2 + 1] = min(ph[node * 2 + 1], ph[node]);
        else
            ph[node * 2 + 1] = ph[node];
    }
    ph[node] = -1;
}

void count(int node) {
    if (node >= R)
        return;
    if (tr[node * 2].S == INF && tr[node * 2 + 1].S == INF)
        tr[node] = {INF, INF};
    else if (tr[node * 2].S == INF)
        tr[node] = tr[node * 2 + 1];
    else if (tr[node * 2 + 1].S == INF)
        tr[node] = tr[node * 2];
    else
        tr[node] = min(tr[node * 2], tr[node * 2 + 1]);
}

void set_ind(int ind, ll val, int node = 1, int nl = 0, int nr = R - 1) {
    push(node);
    if (ind < nl || ind > nr)
        return;
    if (nl == ind && ind == nr) {
        tr[node].F = val;
        if (val == INF)
            tr[node].S = INF;
        return;
    }
    int nm = (nl + nr) / 2;
    set_ind(ind, val, node * 2, nl, nm);
    set_ind(ind, val, node * 2 + 1, nm + 1, nr);
    count(node);
}

void modify(int ql, int qr, ll val, int node = 1, int nl = 0, int nr = R - 1) {
    push(node);
    if (qr < nl || nr < ql)
        return;
    if (ql <= nl && nr <= qr) {
        ph[node] = val;
        push(node);
        return;
    }
    int nm = (nl + nr) / 2;
    modify(ql, qr, val, node * 2, nl, nm);
    modify(ql, qr, val, node * 2 + 1, nm + 1, nr);
    count(node);
}

bool used[MAXN];
ll Ps1[MAXN], Ps2[MAXN];

// #define LOCAL

void solve() {
    int n, k;
    cin >> n >> k;

    fill(used, used + n, false);
    fill(dist, dist + n, INF);
    dist[0] = 0;
    mp.clear();
    fill(s, s + n, "");
    szb = 1, start_do = 0;
    b.clear();
    b.push_back({});

    for (int i = 0; i < n; i++)
        cin >> s[i];

    for (int i = 0; i < n; i++)
        b_add(i, s[i]);

    b_get_sorted();

    R = 1;
    while (R <= start_do)
        R *= 2;

    build();
    for (int i = 0; i < start_do; i++)
        if (b[ido_to_ib[i]].tm[i - b[ido_to_ib[i]].l] == 0)
            set_ind(i, 0);

    while (true) {
        if (tr[1].F == INF || tr[1].S == INF)
            break;
        ll ind = tr[1].S;
        int ibg = ido_to_ib[ind];
        int is = b[ibg].tm[ind - b[ibg].l];
        dist[is] = min(tr[1].F, dist[is]);
        set_ind(ind, INF);
        if (used[is])
            continue;
        used[is] = true;
        if (dist[is] == INF)
            continue;
#ifdef LOCAL
        cerr << is << ' ' << dist[is] << endl;
#endif
        pair <ll, ll> h = {0, 0};
        for (int i = s[is].size() - 1; i >= 0; i--) {
            h = {(h.F + ll(s[is][i] - 'a' + 1) * Ps1[s[is].size() - i - 1]) % MOD1,
                   (h.S + ll(s[is][i] - 'a' + 1) * Ps2[s[is].size() - i - 1]) % MOD2};
            if (s[is].size() - i < k)
                continue;
            auto it = mp.find(h);
            if (it == mp.end())
                continue;
            int ib = it->S;
            if (b[ib].flag && i > 0 && s[is][i - 1] == s[is][i]) {
                int l1 = b[ib].l + b[ib].cnt, r1 = b[ib].r;
                for (int j = 0; j < s[is][i - 1] - 'a'; j++)
                    if (b[ib].nxt[j] != -1)
                        l1 += b[b[ib].nxt[j]].tm.size();
                l1--;
                for (int j = 25; j > s[is][i - 1] - 'a'; j--)
                    if (b[ib].nxt[j] != -1)
                        r1 -= b[b[ib].nxt[j]].tm.size();
                r1++;
                if (l1 >= b[ib].l) {
                    modify(b[ib].l, l1, dist[is] + s[is].size() - i);
#ifdef LOCAL
                    cerr << "l " << dist[is] << ' ' << s[is].size() - i << "  ";
                    for (int j = b[ib].l; j <= l1; j++)
                        cerr << b[ib].tm[j - b[ib].l] << ' ';
                    cerr << endl;
#endif
                }
                if (b[ib].r >= r1) {
                    modify(r1, b[ib].r, dist[is] + s[is].size() - i);
#ifdef LOCAL
                    cerr << "r " << dist[is] << ' ' << s[is].size() - i << "  ";
                    for (int j = r1; j <= b[ib].r; j++)
                        cerr << b[ib].tm[j - b[ib].l] << ' ';
                    cerr << endl;
#endif
                }
            } else {
                modify(b[ib].l, b[ib].r, dist[is] + s[is].size() - i);
#ifdef LOCAL
                cerr << "no flag " << dist[is] << ' ' << s[is].size() - i << "  ";
                for (int j = b[ib].l; j <= b[ib].r; j++)
                    cerr << b[ib].tm[j - b[ib].l] << ' ';
                cerr << endl;
#endif
            }
        }
    }

    for (int i = 1; i < n; i++)
        cout << (dist[i] == INF ? -1 : dist[i]) << ' ';
    cout << endl;
}

signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    auto start_time = chrono::high_resolution_clock::now();

    Ps1[0] = Ps2[0] = 1;
    for (int i = 1; i < MAXN; i++) {
        Ps1[i] = Ps1[i - 1] * P1 % MOD1;
        Ps2[i] = Ps2[i - 1] * P2 % MOD2;
    }

    int tt = 1;
    cin >> tt;
    for (int _ = 0; _ < tt; _++)
        solve();

    auto end_time = chrono::high_resolution_clock::now();
    cerr << "Execution time: " <<
            chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << " ms" << endl;
}
