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
bool comp(pair<ll, ll> x, pair<ll, ll> y) {
    return x.first * y.second > y.first * x.second;
}
int main() {
    ll m;
    cin >> m;
    vector<pair<ll, ll>> cards;
    ll a;
    ll cost = 0;
    for (int i = 0; i < 31; i++) {
        cin >> a;
        cards.push_back(make_pair(a, pow(2, i)));
    }
    sort(cards.begin(), cards.end(), comp);
    ll n;
    ll mn = inf;
    for (int i = 0; i < 31; i++) {
        n = m / cards[i].first;
        m -= n * cards[i].first;
        cost += n * cards[i].second;
        if (m == 0) {
            mn = min(mn, cost);
            break;
        }
        mn = min(mn, cost + cards[i].second);
    }
    cout <<mn;
}