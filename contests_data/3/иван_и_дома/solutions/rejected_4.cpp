#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <cassert>
#include <math.h>

using namespace std;

bool full_square_check (long long x) {
    long long sx = sqrt(x);
    for (long long k = max(0ll, sx - 2); k <= sx + 2; ++k) {
        if (k * k == x) {
            return true;
        }
    }
    return false;
}

bool all_segments_check (vector<int> &a, int l, int r) {
    for (int i = l; i <= r; ++i) {
        long long prod = 1;
        for (int j = i; j <= r; ++j) {
            prod *= a[j];
            if (full_square_check(prod)) {
                return false;
            }
        }
    }

    return true;
}

bool borders_check (vector<int> &a, vector<int> &borders) {
    int l = 0;
    for (auto r: borders) {
        if (!all_segments_check(a, l, r)) {
            return false;
        }
        l = r + 1;
    }
    return true;
}

bool get_borders (int n, int cnt_segs, vector<int> &a, vector<int> &borders) {
    if (cnt_segs == 1) {
        borders.push_back(n - 1);
        if (borders_check(a, borders)) {
            return true;
        }
        borders.pop_back();
        return false;
    }

    int start = 0;
    if (!borders.empty()) {
        start = borders.back() + 1;
    }
    for (int b = start; b < n; ++b) {
        if (b == n - 1) {
            return false;
        }
        borders.push_back(b);
        if (get_borders(n, cnt_segs - 1, a, borders)) {
            return true;
        }
        borders.pop_back();
    }
    return false;
}

void solve() {
    int n;
    cin >> n;
    vector<int> a (n);
    for (auto &i : a) {
        cin >> i;
    }

    vector<int> borders;
    for (int cnt_segs = 1; cnt_segs <= n; ++cnt_segs) {
        if (get_borders(n, cnt_segs, a, borders)) {
            cout << cnt_segs << endl;
            return;
        }
    }

    assert(0);
}

int main() {
    solve();
}
