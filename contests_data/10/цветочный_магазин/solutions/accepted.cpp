
#include <bits/stdc++.h>
 
using namespace std;
 
#define int long long

using ld = long double;

struct Flower {
    int a;
    int b;
    int boq;

    bool operator<(const Flower& o) const { 
        return boq < o.boq;
    }
};

const int inf = 3 * 1e18;
 
void run() {
	int n, q;
	cin >> n >> q;

    vector<Flower> flowers(n);
    for (int i = 0; i < n; ++i) {
        cin >> flowers[i].a;
    }
    for (int i = 0; i < n; ++i) {
        flowers[i].b = 1;
        if (flowers[i].b == 0) {
            flowers[i].boq = 0;
        } else {
            flowers[i].boq = flowers[i].a / flowers[i].b;
        }
    }

    sort(flowers.begin(), flowers.end());

    map<int, __int128_t> boquet_cost; // {a:b} starting from boquet a, costs increase by b
    boquet_cost[0] = 0;
    for (int i = 0; i < n; ++i) {
        if (flowers[i].b == 0) continue;
        boquet_cost[flowers[i].boq + 1] += flowers[i].b - flowers[i].a % flowers[i].b;
        boquet_cost[flowers[i].boq + 2] += flowers[i].a % flowers[i].b;
    }
    map<int, __int128_t> boq_cost;  // {a:b} starting from boquet a, cost is now b
    __int128_t prev_cost = 0;
    for (const auto& [boquet, cost] : boquet_cost) {
        // cout << boquet << " " << cost << endl;
        boq_cost[boquet] = prev_cost + cost;
        prev_cost += cost;
    }

    vector<__int128_t> cost, boq;  // {a:b} you need to spend a money to buy b bouqets
    prev_cost = 0;
    __int128_t prev_price = 0;
    __int128_t prev_boq = 0;
    for (const auto& p : boq_cost) {
        __int128_t cboq = p.first, ccost = p.second;
        // cout << cboq << " " << ccost << endl;
        prev_cost += prev_price * (cboq - prev_boq);
        prev_price = ccost;
        prev_boq = cboq;
        cost.push_back(prev_cost + ccost);
        boq.push_back(cboq);
    }

    // for (int i = 0; i < cost.size(); ++i) {
    //     cout << cost[i] << " " << boq[i] << endl;
    // }

    for (int i = 0; i < q; ++i) {
        int xx;
        cin >> xx;
        __int128_t x = xx;
        int pos = upper_bound(cost.begin(), cost.end(), x) - cost.begin();
        assert(pos > 0);
        --pos;

        x -= cost[pos];
        if (pos + 1 == boq.size()) {
            cout << static_cast<int>(boq[pos] + x / boq_cost[boq[pos]]) << " ";
        } else if (pos == 0) {
            cout << static_cast<int>(boq[1] - 1) << " ";
        } else {
            cout << static_cast<int>(min(boq[pos] + x / boq_cost[boq[pos]], boq[pos + 1] - 1)) << " ";
        }
    }
}

signed main() {
    ios::sync_with_stdio(0);
    cin.tie();
    cout << fixed << setprecision(13);
    int t = 1;
    // cin >> t;
    for (int test = 1; test <= t; ++test) {
        run();
    }
}
