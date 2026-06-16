#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stack>
#include <queue>
#include <set>
#include <deque>
#include <iomanip>

using namespace std;

int main() {
    double mid, c, EPS, l, r;
    cin >> c;
    EPS = 1e-7;
    l = EPS;
    r = 1e5;
    for (int i = 0; i < 100; i++) {
        mid = (l + r) / 2;
        if (c > mid * mid + sqrt(mid)) {
            l = mid;
        } else {
            r = mid;
        }
    }
    cout << fixed << setprecision(6) << l;
}