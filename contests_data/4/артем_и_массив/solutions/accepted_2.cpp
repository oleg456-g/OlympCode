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
    int n; cin >> n;
    vector<int> ps(n);
    vector<int> a(n);
    for (int& x : a) cin >> x;

    for (int i = 1; i < n; ++i)
        ps[i] = a[i] ^ a[i-1];

    cout << n-1 << '\n';
    for (int i = 1; i < n; ++i) {
        int j = i >= (n+1)/2 ? 0 : n;
        cout << min(i,j)+1 << " " << max(i, j) << " " << ps[i] << '\n';
    }
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0);
    int t = 1;
    cin >> t;
    while (t--) solve();
}