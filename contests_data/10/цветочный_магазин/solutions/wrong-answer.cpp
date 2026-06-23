
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
        cin >> flowers[i].b;
        if (flowers[i].b == 0) {
            flowers[i].boq = 0;
        } else {
            flowers[i].boq = flowers[i].a / flowers[i].b;
        }
    }

    sort(flowers.begin(), flowers.end());

    map<int, int> boquet_cost; // {a:b} starting from boquet a, costs increase by b
    boquet_cost[0] = 0;
    for (int i = 0; i < n; ++i) {
        if (flowers[i].b == 0) continue;
        if (flowers[i].a % flowers[i].b > 0) {
            boquet_cost[flowers[i].boq + 1] += flowers[i].a % flowers[i].b;
            boquet_cost[flowers[i].boq + 2] += flowers[i].b - flowers[i].a % flowers[i].b;
        } else {
            boquet_cost[flowers[i].boq + 1] += flowers[i].b;
        }
    }
    map<int, int> boq_cost;  // {a:b} starting from boquet a, cost is now b
    int prev_cost = 0;
    for (const auto& [boquet, cost] : boquet_cost) {
        // cout << boquet << " " << cost << endl;
        boq_cost[boquet] = prev_cost + cost;
        prev_cost += cost;
    }

    vector<int> cost, boq;  // {a:b} you need to spend a money to buy b bouqets
    prev_cost = 0;
    int prev_price = 0;
    int prev_boq = 0;
    for (const auto& [cboq, ccost] : boq_cost) {
        // cout << cboq << " " << ccost << endl;
        if (prev_price > 0 && cboq - prev_boq >= inf / prev_price) { // prev_price * (cboq - prev_boq) < inf
            cost.push_back(inf);
            boq.push_back(cboq);
            break;
        }
        prev_cost += prev_price * (cboq - prev_boq);
        prev_price = ccost;
        prev_boq = cboq;
        cost.push_back(prev_cost);
        boq.push_back(cboq);
    }

    // for (int i = 0; i < cost.size(); ++i) {
    //     cout << cost[i] << " " << boq[i] << endl;
    // }

    for (int i = 0; i < q; ++i) {
        int x;
        cin >> x;
        int pos = upper_bound(cost.begin(), cost.end(), x) - cost.begin();
        assert(pos > 0);
        --pos;

        x -= cost[pos];
        cout << boq[pos] + x / boq_cost[boq[pos]] - 1 << " ";
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
