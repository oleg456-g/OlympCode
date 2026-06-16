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

int lower_bound1(vector<long long>& a, long long p) {
    int l = -1;
    int r = a.size();
    while (r - l > 1) {
        int mid = (r + l) / 2;
        if (a[mid] < p) {
            l = mid;
        } else {
            r = mid;
        }
    }
    return l;
}


int upper_bound1(vector<long long>& a, long long p) {
    int l = -1;
    int r = a.size();
    while (r - l > 1) {
        int mid = (r + l) / 2;
        if (a[mid] <= p) {
            l = mid;
        } else {
            r = mid;
        }
    }
    return r;
}
int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    int n, m;
    long long j;
    cin >> n;
    vector<long long> a(n, 0);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }
    sort(a.begin(), a.end());
    cin >> m;
    for (int i = 0; i < m; i++) {
        cin >> j;
        cout << upper_bound1(a, j) - lower_bound1(a, j) - 1 << ' ';
    }
}