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

void play_alice(int n, int m, vector<int> a, string s) {
    cout << "Alice" << endl;
    const int sum_mod = accumulate(a.begin(), a.end(), 0ll)%m;
    if (s[sum_mod] == '0') {
        cout << "replace " << a[0] << endl;
        cin >> a[0];
    }

    while (a.size() >= 2) {
        int x = a.back(); a.pop_back();
        int y = a.back();
        cout << "unite " << x << " " << y << endl;
        a.back() = (x + y) % m;
    }

    cout << "done" << endl;
}

void play_bob(int n, int m, vector<int> a, string s) {
    cout << "Bob" << endl;
    int sum_mod = accumulate(a.begin(), a.end(), 0ll)%m;

    int x = sum_mod, y = 0;
    while (s[y] == '1' || y == x) ++y;

    while (true) {
        string op; cin >> op;
        if (op == "done") break;

        if (op == "unite") {
            int x, y; cin >> x >> y;
            // ignore :)
        }

        if (op == "replace") {
            int a; cin >> a;
            if (sum_mod == x) {
                a += y - x;
                sum_mod += y - x;
            } else {
                a -= y - x;
                sum_mod -= y - x;
            }
            sum_mod = (sum_mod + m) % m;
            a = (a + m) % m;
            cout << a << endl;
        }
    }
}

void solve() {
    int n, m; cin >> n >> m;
    vector<int> a(n);
    for (int& x : a) cin >> x;
    string s; cin >> s;

    const int sum_mod = accumulate(a.begin(), a.end(), 0ll)%m;
    int cnt = 0;
    for (char c : s) cnt += c - '0';

    if (cnt == m - 1 || s[sum_mod] == '1') {
        play_alice(n, m, a, s);
    } else {
        play_bob(n, m, a, s);
    }
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0);
    int t = 1;
    cin >> t;
    while (t--) solve();
}
