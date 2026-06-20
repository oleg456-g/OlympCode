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
    vec<int> a(2 * n);
    multiset<int> st;
    for (int i = 0; i < n + n; ++i) {
        cin >> a[i];
        st.insert(a[i]);
    }
    int s = 0;
    while ((int)st.size() > 0) {
        auto it = (st.begin());
        cout << (*it) << " ";
        s += (*it);
        st.erase(it);
        st.erase(st.find(s));
    }
    cout << endl;
}

signed main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    int t = 1;
    cin >> t;
    for (; t > 0; --t) solve();
}