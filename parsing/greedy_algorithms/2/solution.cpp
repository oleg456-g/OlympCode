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

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, k;
    cin >> n >> k;
    vector <int> a(k);
    for (int i = 0; i < k; i++) {
        cin >> a[i];
    }
    sort(a.begin(), a.end());
    int cnt = 0;
    while (cnt<k && n>= a[cnt]) {
        n -= a[cnt];
        cnt++;
    }
    cout << cnt;
}