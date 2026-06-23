//#define _GLIBCXX_DEBUG
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <queue>
#include <deque>
#include <string>
#include <cmath>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <cassert>
#include <random>
#include <cstring>
#include <climits>
#include <time.h>
#include <chrono>
#include <fstream>
//#include <bits/stdc++.h>

typedef int_fast64_t ll;
typedef unsigned long long ull;
typedef long double ld;

#define all(a) a.begin(), a.end()
#define rall(a) a.rbegin(), a.rend()
#define print(a) for (auto& _ : a) cout << _ << " ";
#define input(a) for (auto& _ : a) cin >> _;
#define mp make_pair
#define mt make_tuple
#define lb lower_bound
#define ub upper_bound
#define umap unordered_map
#define uset unordered_set
#define mset multiset
#define umset unordered_multiset
#define vll __int128

//#define x first
//#define y second

#define int long long

//std::mt19937 rnd(time(0));
std::mt19937 rnd(52);
using namespace std;

int n;

int gcd(int a, int b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

int f(vector<vector<int>> & a) {
    int res = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i < n - 1 && gcd(a[i][j], a[i + 1][j]) > 1)
                res++;
            if (j < n - 1 && gcd(a[i][j], a[i][j + 1]) > 1)
                res++;
        }
    }
    return res;
}

int f1(vector<vector<int>>& a, int i, int j) {
    int res = 0;
    if (i > 0)
        res += (gcd(a[i][j], a[i - 1][j]) > 1);
    if (i < n - 1)
        res += (gcd(a[i][j], a[i + 1][j]) > 1);
    if (j > 0)
        res += (gcd(a[i][j], a[i][j - 1]) > 1);
    if (j < n - 1)
        res += (gcd(a[i][j], a[i][j + 1]) > 1);
    return res;
}

void solve() {
    cin >> n;

    auto start = chrono::steady_clock::now();
    vector<vector<int>> a(n, vector<int>(n, 1));
    vector<pair<int, int>> c0, c1;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if ((i + j) % 2 == 0)
                c0.push_back({i, j});
            else
                c1.push_back({i, j});
    vector<pair<int, int>> odd_cells = c0, even_cells = c1;
    if (odd_cells.size() < even_cells.size())
        swap(odd_cells, even_cells);
    vector<int> odd, even;
    for (int x = 1; x <= n * n; ++x)
        if (x % 2 == 1)
            odd.push_back(x);
        else
            even.push_back(x);
    shuffle(all(odd), rnd);
    shuffle(all(even), rnd);
    for (int i = 0; i < odd_cells.size(); ++i)
        a[odd_cells[i].first][odd_cells[i].second] = odd[i];
    for (int i = 0; i < even_cells.size(); ++i)
        a[even_cells[i].first][even_cells[i].second] = even[i];
    int res = f(a);
    vector<vector<int>> best_a = a;
    int best_res = res;
    int k = 0;
    while (res > 0 && chrono::duration<double>(chrono::steady_clock::now() - start).count() < 1.9) {
        int i1 = rnd() % n, j1 = rnd() % n, i2 = rnd() % n, j2 = rnd() % n;
        if (i1 == i2 && j1 == j2)
            continue;
        if ((i1 + j1) % 2 != (i2 + j2) % 2)
            continue;
        int last = f1(a, i1, j1) + f1(a, i2, j2);
        if (abs(i1 - i2) + abs(j1 - j2) == 1 && gcd(a[i1][j1], a[i2][j2]) > 1)
            last--;
        swap(a[i1][j1], a[i2][j2]);
        int x = f1(a, i1, j1) + f1(a, i2, j2);
        if (abs(i1 - i2) + abs(j1 - j2) == 1 && gcd(a[i1][j1], a[i2][j2]) > 1)
            x--;
        ld y = (ld)(rnd() % (int)(1e9)) / 1e9, z = exp(-(ld)k / 1e7 * 2.28);
        if (last < x)
            swap(a[i1][j1], a[i2][j2]);
        else
            res += x - last;
        if (res < best_res) {
            best_res = res;
            best_a = a;
        }
        k++;
    }
    for (int i = 0; i < n; ++i) {
        print(best_a[i]);
        cout << "\n";
    }
}

signed main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    int t;
    cin >> t;
    while (t--)
        solve();
    
    return 0;
}
