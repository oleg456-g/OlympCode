#include <bits/stdc++.h>
using namespace std;

struct Coworking {
    long long a, o, c;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    vector<Coworking> v(n);
    for (int i = 0; i < n; ++i) {
        cin >> v[i].a >> v[i].o >> v[i].c;
        // v[i].c++; // DELETE ME!!!!!1111
    }
    sort(v.begin(), v.end(), [](const Coworking& x, const Coworking& y) {
        return x.c < y.c;
    });

    const long long NEG = LLONG_MIN / 4;
    vector<long long> dp(n, NEG);
    long long ans = 0;

    for (int i = 0; i < n; ++i) {
        // From start (position 0, time 0)
        long long arrivalFromStart = v[i].a;               // travel time
        long long startStay = max(v[i].o, arrivalFromStart);
        long long stay = v[i].c - startStay;
        if (stay > 0) dp[i] = stay;

        // Transitions from previous coworkings
        for (int j = 0; j < i; ++j) {
            if (dp[j] == NEG) continue;
            long long arrival = v[j].c + llabs(v[i].a - v[j].a);
            if (arrival > v[i].c) continue; // cannot get any time
            long long start = max(v[i].o, arrival);
            long long add = v[i].c - start;
            if (add <= 0) continue;
            dp[i] = max(dp[i], dp[j] + add);
        }
        ans = max(ans, dp[i]);
    }

    cout << ans << '\n';
    return 0;
}
