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
    int n, v, s;
    cin >> n >> v >> s;
    vector <int> a(s+1, 0);
    for (int i = 1; i < s+1; i++) {
        cin >> a[i];
    }
    a.push_back(n);
    vector <int> b;
    int k;
    for (int i = 0; i < int(a.size())-1; i++) {
        k = abs(a[i] - a[i + 1]);
        if (k > v) {
            cout << "-1";
            return 0;
        }
        b.push_back(k);
    }
    int fuel = v;
    int cnt = 0;
    for (int i = 0; i < int(b.size())-1; i++) {
        fuel -= b[i];
        if (fuel < b[i + 1]) {
            fuel = v;
            cnt++;
        }
    }
    cout << cnt;
}