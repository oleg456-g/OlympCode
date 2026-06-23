#include <iostream>
#include <vector>
using namespace std;

long long x, y, z;
vector<long long> s(2), c(2);
long long a, b;

bool is_exists = false;
long long ans = 1e18;

void solve(int door_wall, int num1, int num2) { // куда ставим дверь, какие обои используем
    vector<long long> sum(2, 0); // сколько какого вида обоев используеться

    if (door_wall == 0) {
        if (z < b || x < a) { // влезает ли дверь
            return;
        }
        sum[num1] -= a * b;
    }
    else {
        if (z < b || y < a) { // влезает ли дверь
            return;
        }
        sum[num2] -= a * b;
    }

    sum[num1] += 2 * x * z;
    sum[num2] += 2 * y * z;

    if (sum[0] <= s[0] && sum[1] <= s[1]) {
        is_exists = true;
        ans = min(ans, sum[0] * c[0] + sum[1] * c[1]);
    }
}

int main() {
    cin >> x >> y >> z;

    cin >> s[0] >> c[0] >> s[1] >> c[1];

    cin >> a >> b;

    vector<int> mask = { 0, 0, 0 };

    for (int i = 0; i < 8; i++) {
        solve(mask[0], mask[1], mask[2]);
        for (auto& el : mask) {
            if (el == 0) {
                el = 1;
                break;
            }
            else {
                el = 0;
            }
        }
    }

    if (!is_exists) {
        cout << "-1\n";
    }
    else {
        cout << ans << "\n";
    }
}
