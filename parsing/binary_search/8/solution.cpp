#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stack>
#include <queue>
#include <set>
#include <deque>
#include <iomanip>

using namespace std;

bool isOK(long long distance, int n, int m, vector<long long> a) {
    long long last_remembered = a[0];
    int counter = 1;
    for (int i = 0; i < n; i++) {
        if (last_remembered + distance <= a[i]) {
            counter++;
            last_remembered = a[i];
        }
    }
    return counter >= m;
}

int main() {
    int n, m;
    long long l, r, mid;
    cin >> n >> m;
    vector<long long> a(n, 0);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }
    l = 0;
    r = 1e10;
    while (r - l > 1) {
        mid = (r + l) / 2;
        if (isOK(mid, n, m, a)) {
            l = mid;
        } else {
            r = mid;
        }
    }
    cout << l;
}