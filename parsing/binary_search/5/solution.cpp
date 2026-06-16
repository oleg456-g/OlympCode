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
double f(long long a, long long b, long long c, long long d, double x) {
    return a * pow(x, 3) + b * pow(x, 2) + c * x + d;
}

int main() {
    long long a, b, c, d;
    double mid, EPS, l, r;
    cin >> a >> b >> c >> d;
    EPS = 1e-5;
    l = -10e10;
    r = 1e10;
    while (r - l > EPS) {
        mid = (l + r) / 2;
        if ((f(a, b, c, d, mid) > 0 && f(a, b, c, d, l) < 0) || (f(a, b, c, d, mid) < 0 && f(a, b, c, d, l) > 0)) {
            r = mid;
        } else {
            l = mid;
        }
    }
    cout << fixed << setprecision(4) << l;
}