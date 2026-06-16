#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

int main() {
    int n, m, k, j, i, counter;
    cin >> n >> m;
    vector<long long> a(n * m, 0);

    for (int i = 0; i < n * m; i++) {
        cin >> a[i];
    }
    cin >> k;
    vector<long long> b(k, 0);
    for (int i = 0; i < k; i++) {
        cin >> b[i];
    }
    sort(a.begin(), a.end());
    sort(b.begin(), b.end());
    counter = 0;
    j = 0;
    i = 0;
    while (i < k) {
        if (j < n * m && i < k && a[j] >= b[i]) {
            counter++;
            j++;
            i++;
        } else if (j < n * m && i < k && a[j] < b[i]) {
            j++;
        } else {
            break;
        }
    }
    cout << counter;
}