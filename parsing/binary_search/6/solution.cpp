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

bool OK(vector<long long> a, long long l, int k) {
    long long summa;
    summa = 0;
    for (int i = 0; i < a.size(); i++) {
        summa += (a[i] / l);
    }
    return summa >= k;
}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    int n, k;
    long long l, r, mid;
    cin >> n >> k;
    vector<long long> a(n, 0);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }
    l = 0;
    r = 1e9;
    while (r - l > 1) {
        mid = (l + r) / 2;
        if (OK(a, mid, k)) {
            l = mid;
        } else {
            r = mid;
        }
    }
    cout << l;
}