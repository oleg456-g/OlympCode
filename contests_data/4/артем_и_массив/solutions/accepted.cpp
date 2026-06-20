#include <iostream>
#include <bitset>
#include <list>
#include <new>
#include <math.h>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <string>
#include <deque>
#include <queue>
#include <random>
#include <utility>
#include <iterator>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <functional>
#include <fstream>

using namespace std;

typedef long long ll;
typedef long double ld;
typedef pair<int, int> pii;

#define vec vector
#define endl '\n'
#define int long long

#pragma GCC optimize("O3,unroll-loops")
#pragma GCC target("avx2")

const int INF = 2e9 + 333, INF2 = 2e9 + 333, p_1 = 31, p_2 = 37;

void solve() {
    int n;
    cin >> n;
    vec<int> a(n);
    int s = 0;
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }
    cout << n - 1 << endl;
    for (int i = 1; i < (n + 1) / 2; ++i) {
        cout << i + 1 << " " << n << " " << ((a[i] ^ a[0]) ^ s) << endl;
        s ^= ((a[i] ^ a[0]) ^ s);
    }
    for (int i = (n + 1) / 2; i < n; ++i)  {
        cout << 1 << " " << i << " " << ((a[i] ^ a[0]) ^ s) << endl;
        s ^= ((a[i] ^ a[0]) ^ s);
    }
}

signed main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    int t = 1;
    cin >> t;
    for (; t > 0; --t) solve();
}