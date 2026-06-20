#include <bits/stdc++.h>
#include <numeric>
#include <queue>

using namespace std;
using ll = long long;
using pi = pair<int, int>;

template<typename T>
bool ckmin(T& a, const T& b) { return b < a ? a = b, true : false; }
template<typename T>
bool ckmax(T& a, const T& b) { return a < b ? a = b, true : false; }

template<typename T>
using pq_min = priority_queue<T, vector<T>, greater<T>>;
template<typename T>
using pq_max = priority_queue<T, vector<T>, less<T>>;

struct Tree {
    int n;
    vector<int> p, w, is_leaf;
    vector<vector<int>> go;

    void Dfs(int v) {
        is_leaf[v] = go[v].empty();

        for (int to : go[v]) {
            w[to] ^= w[v];
            Dfs(to);
        }
    }

    void Read(istream& is) {
        is >> n;
        p.resize(n);
        w.resize(n);
        go.resize(n);
        is_leaf.resize(n);
        for (int& x : w) cin >> x;
        for (int& x : p) cin >> x, --x;

        for (int i = 0; i < n; ++i)
            if (i != p[i]) go[p[i]].push_back(i);

        for (int i = 0; i < n; ++i) {
            if (i == p[i]) Dfs(i);
        }
    }
};

ll solve(const Tree& s, const Tree& t) {
    constexpr int K = 30;
    array<ll, K> bits;
    fill(bits.begin(), bits.end(), 0);

    int tleafs = 0;
    for (int i = 0; i < t.n; ++i) {
        if (!t.is_leaf[i]) continue;
        ++tleafs;
        for (int j = K; j--; ) bits[j] += (t.w[i] >> j) & 1;
    }

    // cout << "W(S) = ";
    // for (int i = 0; i < s.n; ++i) cout << s.w[i] << " ";
    // cout << endl;
    // cout << "W(T) = ";
    // for (int i = 0; i < t.n; ++i) cout << t.w[i] << " ";
    // cout << endl;
    // cout << "bits = ";
    // for (int i = 0; i < 6; ++i) cout << bits[i] << " ";
    // cout << endl;

    ll ans = 0;

    ll sum = 0;
    for (int i = 0; i < s.n; ++i) {
        if (s.is_leaf[i]) sum += s.w[i];
    }

    for (int i = 0; i < s.n; ++i) {
        ll cur = sum;
        if (s.is_leaf[i]) cur -= s.w[i];

        for (int j = K; j--; ) {
            if ((s.w[i] >> j) & 1) {
                cur += (tleafs - bits[j]) << j;
            } else {
                cur += bits[j] << j;
            }
        }

        // cout << "tree #" << i+1 << ": " << cur << endl;

        ckmax(ans, cur);
    }

    return ans;
}

void solve() {
    Tree s, t;
    s.Read(cin);
    t.Read(cin);
    ll ans = max(solve(s, t), solve(t, s));
    cout << ans << '\n';
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0);
    int t = 1;
    cin >> t;
    while (t--) solve();
}