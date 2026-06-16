#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stack>
#include <queue>
#include <set>
#include <deque>
#include <string>
#include <numeric>

using namespace std;
vector <int> b;
bool check(int mid, vector<vector<int>> a, int m) {
    b.clear();
    for (int i = 0; i < a.size(); i++) {
        b.push_back(min(mid / (a[i][0] * a[i][1] + a[i][2]) * a[i][1] + min(mid % (a[i][0] * a[i][1] + a[i][2]) / a[i][0], a[i][1]), m));
        m -= b[i];
    }

    if (m > 0) {
        return false;
    } else {
        return true;
    }
}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    int n, m;
    cin >> m >> n;
    vector <int> c;
    vector<vector<int>> a(n, vector<int>(3, 0));
    for (int i = 0; i < n; i++) {
        cin >> a[i][0] >> a[i][1] >> a[i][2];
    }
    long l, r;
    l = -1;
    r = 1e7;
    while (r - l > 1) {
        long mid = (l + r) / 2;
        if (check(mid, a, m)) {
            r = mid;
            c = b;
        } else {
            l = mid;
        }
    }
    cout << r << '\n';
    if (r == 0) {
        c.clear();
        vector <int> c(n, 0);
    }
    for (int i = 0; i < n; i++) {
        cout << c[i] << ' ';
    }
}