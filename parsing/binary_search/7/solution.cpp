#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stack>
#include <queue>
#include <set>
#include <deque>
#include <string>
#include <numeric>

using namespace std;

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    int x, y;
    long long l, r, mid, n;
    cin >> n >> x >> y;
    l = 0;
    r = 1e9;
    if (y < x) {
        swap(x, y);
    }
    while (r - l > 1) {
        mid = (l + r) / 2;
        if (((mid - x) / x) + ((mid - x) / y) >= n - 1) {
            r = mid;
        } else {
            l = mid;
        }
    }
    if (n == 1) {
        cout << x;
    } else {
        cout << r;
    }
}