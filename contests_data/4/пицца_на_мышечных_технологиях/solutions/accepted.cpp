#include <bits/stdc++.h>
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

void solve() {
    int n, k; cin >> n >> k;
    vector<int> a(n);
    for (int& x : a) cin >> x;

    ll ans = 0;
    int i = 0, j = n - 1;
    while (j - i > k) {
        int d = min(a[i], a[j]);
        ans += d;
        a[i] -= d;
        a[j] -= d;
        if (!a[i]) ++i;
        if (!a[j]) --j;
    }
    cout << 1+ans << '\n';
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0);
    int t = 1;
    cin >> t;
    while (t--) solve();
}