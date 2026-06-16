#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cmath>
#include <string>
#include <queue>
typedef long long ll;
ll inf = 1000000000000000000;
using namespace std;
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    ll n, a;
    cin >> n;
    ll ans = 0;
    priority_queue<ll, vector<ll>, greater<ll>> pq;
    for (ll i = 0; i < n; i++) {
        cin >> a;
        pq.push(a);
    }
    ll k;
    while (int(pq.size()) > 1) {
        k = 0;
        k += pq.top();
        pq.pop();
        k += pq.top();
        pq.pop();
        ans += k;
        pq.push(k);
    }
    cout << ans;
}