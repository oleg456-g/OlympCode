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
    vector<int> b(2*n);
    for (int& x : b) cin >> x;
    sort(b.begin(), b.end());

    vector<int> used(2*n), a;
    int i = 0, j = 0, s = 0;
    while ((int)a.size()<n) {
        while (used[i]) ++i;
        j = max(j, i+1);
        s += b[i];
        while (b[j] < s) ++j;

        used[i] = used[j] = 1;
        a.push_back(b[i]);

    }

    for (int x : a) cout << x << ' ';
    cout << '\n';
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0);
    int t = 1;
    cin >> t;
    while (t--) solve();
}