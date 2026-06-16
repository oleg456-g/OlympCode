#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    int n, k;
    vector <int> b(20001, 0);
    cin >> n;
    for (int i = 0; i < n; i++) {
        cin >> k;
        b[k + 10000]++;
    }
    for (int i = 0; i <= 20000; i++) {
        while (b[i] > 0) {
            b[i]--;
            cout << i - 10000 << ' ';
        }
    }
}