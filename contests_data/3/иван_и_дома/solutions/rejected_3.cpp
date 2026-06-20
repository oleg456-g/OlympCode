#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <cassert>
#include<random>
#include<ctime>
#include <set>

using namespace std;

mt19937 rnd(time(NULL));

void solve() {
    int n;
    cin >> n;
    vector<int> a (n);
    for (auto &i : a) {
        cin >> i;
        assert(2 <= i && i <= 3);
    }

    set<pair<int, int>> s;
    s.insert({0, 0});
    int ans = 0;
    int has2 = 0, has3 = 0;

    for (auto &i : a) {
        if (i == 2) {
            has2 ^= 1;
        } else {
            has3 ^= 1;
        }
        if (s.contains({has2, has3})) {
            s.clear();
            ++ans;
            has2 = has3 = 0;
            if (i == 2) {
                has2 ^= 1;
            } else {
                has3 ^= 1;
            }
            s.insert({0, 0});
        }
        s.insert({has2, has3});
    }

    cout << ans + 1 << '\n';
}

/**

*/

int main() {
    solve();
}
