#include "bits/stdc++.h"
using namespace std;
 
#define all(x) x.begin(), x.end()
#define fi first
#define se second
#define endl '\n'

const int INF = 1000'000'000;

vector<vector<pair<int, int>>> g;
vector<pair<string, int>> s;
int siz = 1;

void init(int n) {
    while (siz < n) {
        siz <<= 1;
    }
    g.assign(2 * siz, {});
    for (int i = 0; i < siz - 1; i++) {
        g[i].push_back({2 * i + 1, 0});
        g[i].push_back({2 * i + 2, 0});
    }
}

void add_edges(int v, int w, int l, int r, int x, int lx, int rx) {
    if (l <= lx && rx <= r) {
        g[v + siz - 1].push_back({x, w});
        return;
    }
    if (r <= lx || rx <= l) {
        return;
    }
    int m = (lx + rx) / 2;
    add_edges(v, w, l, r, 2 * x + 1, lx, m);
    add_edges(v, w, l, r, 2 * x + 2, m, rx);
}
 
void add_edges(int v, int w, int l, int r) {
    if (r < l) {
        return;
    }
    add_edges(v, w, l, r + 1, 0, 0, siz);
}

int lcp(int i, int j, int suf) {
    int n = (int) s[i].fi.size();
    int m = (int) s[j].fi.size();
    for (int k = 0; k < n && suf + k < m; k++) {
        if (s[i].fi[k] != s[j].fi[suf + k]) {
            return k;
        }
    }
    return min(n, m - suf);
}

int lower_bound(int ind, int suf) {
    int l = -1;
    int r = (int) s.size();
    int k = (int) s[ind].fi.size() - suf;
    while (r - l > 1) {
        int m = (l + r) / 2;
        
        int cur_lcp = lcp(m, ind, suf);
        
        if (cur_lcp < k && (cur_lcp == s[m].fi.size() || s[ind].fi[suf + cur_lcp] > s[m].fi[cur_lcp])) {
            l = m;
        } else {
            r = m;
        }
    }
    return r;
}
 
int upper_bound(int ind, int suf) {
    int l = -1;
    int r = (int) s.size();
    int k = (int) s[ind].fi.size() - suf;
    while (r - l > 1) {
        int m = (l + r) / 2;
        
        int cur_lcp = lcp(m, ind, suf);
        
        if (cur_lcp < k && cur_lcp < s[m].fi.size() && s[ind].fi[suf + cur_lcp] < s[m].fi[cur_lcp]) {
            r = m;
        } else {
            l = m;
        }
    }
    return l;
}

int last_l = -1;
int last_r = -1;

void add_seg(int ind, int suf) {
    int l = lower_bound(ind, suf);
    int r = upper_bound(ind, suf);
    
    if (l == (int) s.size() || l == -1) {
        return;
    }
    
    if (r == (int) s.size() || r == -1) {
        return;
    }
    
    int k = (int) s[ind].fi.size() - suf;
    
    if (lcp(l, ind, suf) == k && lcp(r, ind, suf) == k && l <= r) {
        
        if (last_l == -1 && last_r == -1) {
            add_edges(ind, k, l, r);
        } else if (last_r < l || r < last_l) {
            add_edges(ind, k, l, r);
        }
        else if (last_l <= l && r <= last_r) {
            // pass
        } else if (last_l <= l && l <= last_r) {
            add_edges(ind, k, last_r + 1, r);
        } else if (last_l <= r && r <= last_r) {
            add_edges(ind, k, l, last_l - 1);
        } else {
            add_edges(ind, k, l, last_l - 1);
            add_edges(ind, k, last_r + 1, r);
        }
        
        last_l = l;
        last_r = r;
    } else {
        last_l = -1;
        last_r = -1;
    }
}

void solve() {
    int n, k;
    cin >> n >> k;
    
    s.assign(n, {});
    
    for (int i = 0; i < n; i++) {
        cin >> s[i].fi;
        s[i].se = i;
    }
    
    sort(all(s));
    
    init(n);
    
    for (int i = 0; i < n; i++) {
        int len = (int) s[i].fi.size();
        last_l = -1;
        last_r = -1;
        for (int suf = 0; suf <= min(len - 1, len - k); suf++) {
            add_seg(i, suf);
        }
    }
    
    int start = 0;
    for (int i = 0; i < n; i++) {
        if (s[i].se == 0) {
            start = i;
            break;
        }
    }
    
    
    vector<int> dist(2 * siz, INF);
    dist[start + siz - 1] = 0;
    multiset<pair<int, int>> st;
    st.insert({0, start + siz - 1});
    
    vector<int> par(2 * siz);
    
    while (!st.empty()) {
        int ind = st.begin()->se;
        st.erase(st.begin());
        for (auto [to, w] : g[ind])
            if (dist[to] > dist[ind] + w) {
                par[to] = ind;
                st.erase({dist[to], to});
                dist[to] = dist[ind] + w;
                st.insert({dist[to], to});
            }
    }
        
    vector<int> ansik(n);
    for (int i = 0; i < n; i++) {
        ansik[s[i].se] = dist[i + siz - 1];
    }
    
    for (int i = 1; i < n; i++) {
        cout << (ansik[i] == INF ? -1 : ansik[i]) << ' ';
    }
    cout << endl;
}
 
signed main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T = 1;
    cin >> T;
    while (T--) {
        solve();
    }
}

/*
 1
 10 1
 sstppqq
 qssqsqrptrrstrqs
 rsrrppsrsspt
 ptppsps
 qqrrpqq
 prsqrtqtpt
 tsqrqprrttp
 ptstsprrsts
 srptrtqtrt
 tqrqrrsrr
 */
