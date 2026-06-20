// INCORRECT SOLUTION! TEMPORARILY SET CORRECT
#include <iostream>
#include <vector>
#include <set>

using namespace std;

void solve() {
    int n; cin >> n;
    vector<int> a(n);
    for (auto &el : a) cin >> el;

    set<set<int>> st;
    st.insert(set<int>{});

    // O(n*log(n) * C / log(C)), если a[i] <= C

    int ans = 0;
    set<int> fact;
    for (int i = 0; i < n; ++i) {
        int v = a[i];

        for (int x = 2; x * x <= v; ++x) {
            int c = 0;
            while (v % x == 0) {
                c ^= 1;
                v /= x;
            }

            if (c) {
                if (fact.contains(x)) {
                    fact.erase(x);
                } else {
                    fact.insert(x);
                }
            }
        }

        if (v > 1) {
            if (fact.contains(v)) {
                fact.erase(v);
            } else {
                fact.insert(v);
            }
        }


        if (st.contains(fact)) {
            ans += 1;
            st.clear();
            st.insert(fact);
        } else {
            st.insert(fact);
        }
    }
    cout << ans + 1 << '\n';
}

int main() {
    solve();
}
