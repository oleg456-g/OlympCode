#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using ld = long double;

#define all(a) begin(a), end(a)
#define len(a) int((a).size())

int main() {
    cin.tie(nullptr)->sync_with_stdio(false);

    int x, y, z;
    cin >> x >> y >> z;
    ll s1, c1, s2, c2;
    cin >> s1 >> c1 >> s2 >> c2;
    ll a, b;
    cin >> a >> b;

    constexpr ll INF = 1e18;
    ll ans = INF;
    for (int rot1 = 0; rot1 < 2; rot1++, swap(x, y)) {
        for (int rot2 = 0; rot2 < 2; rot2++, swap(s1, s2), swap(c1, c2)) {
            if (b > z || a > x) {
                continue;
            }
            ll area1 = 2 * y * z;
            ll area2 = 2 * x * z - a * b;
            if (s1 >= area1 && s2 >= area2) {
                ans = min(ans, area1 * c1 + area2 * c2);
            }
            if (s1 >= area1 + area2) {
                ans = min(ans, (area1 + area2) * c1);
            }
        }
    }

    if (ans == INF) {
        ans = -1;
    }
    cout << ans << '\n';
}
