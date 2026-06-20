#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <cassert>
#include <random>
#include <ctime>
#include <set>

using namespace std;

mt19937 rnd(time(NULL));

const int MAXC = 1e6 + 10;
int prime[MAXC];
unsigned long long hash1[MAXC], hash2[MAXC];

void solve() {
    int n;
    cin >> n;
    vector<int> a (n);
    for (auto &i : a) {
        cin >> i;
    }

    set<pair<unsigned long long, unsigned long long>> hashes;
    hashes.insert({0, 0});
    int ans = 0;
    set<int> fact;
    unsigned long long curhash1 = 0, curhash2 = 0;

    for (int i = 0; i < n; ++i) {
        auto jopka = [&](int v) {
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
                    curhash1 ^= hash1[x];
                    curhash2 ^= hash2[x];
                }
            }

            if (v > 1) {
                if (fact.contains(v)) {
                    fact.erase(v);
                } else {
                    fact.insert(v);
                }
                curhash1 ^= hash1[v];
                curhash2 ^= hash2[v];
            }
        };
        jopka(a[i]);

        if (hashes.contains({curhash1, curhash2})) {
            ans += 1;
            hashes.clear();
            hashes.insert({0, 0});
            fact.clear();
            curhash1 = curhash2 = 0;
            jopka(a[i]);
            hashes.insert({curhash1, curhash2});
        } else {
            hashes.insert({curhash1, curhash2});
        }
    }

    cout << ans + 1 << '\n';
}

/**

*/

int main() {
    prime[0] = prime[1] = 1;
    for (int i = 2; i < MAXC; i++) {
        if (prime[i] == 0) {
            for (int j = i; j < MAXC; j += i) {
                prime[j] = 1;
            }
            hash1[i] = rnd();
            hash2[i] = rnd();
        }
    }
    solve();
}
