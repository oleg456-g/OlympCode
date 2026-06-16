#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cmath>
#include <string>
#include <deque>
typedef long long ll;
ll inf = 1000000000000000000;
using namespace std;
bool comp(pair<ll, ll> x, pair <ll, ll> y) {
    return x.second < y.second;
}
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    ll n;
    cin >> n;
    vector < pair<ll, ll>> a;
    ll k, l;
    for (int i = 0; i < n; i++) {
        cin >> k >> l;
        a.push_back(make_pair(k, l));
    }
    sort(a.begin(), a.end(), comp);
    ll last = a[0].second;
    ll cnt = 1;
    for (int i = 1; i < n; i++) {
        if (a[i].first >= last) {
            last = a[i].second;
            cnt++;
        }
    }
    cout << cnt;
}