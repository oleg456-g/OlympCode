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
long long w, h, n;
long long bin(long long a, long long b) {
    long long l = 0;
    long long r = min(w, h);
    while (r - l > 1) {
        long long mid = (r + l) / 2;
        if ((w / (a + 2 * mid)) * (h / (b + 2 * mid)) >= n) {
            l = mid;
        } else {
            r = mid;
        }
    }
    return l;
}
int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    long long a, b;
    cin >> n >> a >> b >> w >> h;
    cout << max(bin(a, b), bin(b, a));
}