#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stack>
#include <queue>
#include <set>
#include <deque>
#include <string>

using namespace std;
long long square(vector<int> a, int n) {
    long long maximum = 0;
    vector<int> res1(n, 0);
    stack<int> st1;
    vector<int> res2(n, 0);
    stack<int> st2;
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }
    for (int i = n - 1; i >= 0; i--) {
        while (!st1.empty() && a[st1.top()] >= a[i]) {
            st1.pop();
        }
        if (st1.empty()) {
            res1[i] = n;
        } else {
            res1[i] = st1.top();
        }
        st1.push(i);
    }

    for (int i = 0; i < n; i++) {
        while (!st2.empty() && a[st2.top()] >= a[i]) {
            st2.pop();
        }
        if (st2.empty()) {
            res2[i] = -1;
        } else {
            res2[i] = st2.top();
        }
        st2.push(i);
    }
    for (int i = 0; i < n; i++) {
        if (maximum < (res1[i] - res2[i] - 1) * a[i]) {
            maximum = a[i] * (res1[i] - res2[i] - 1);
        }
    }
    return maximum;
}
int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    int n, m;
    long long maximum = 0;
    cin >> n >> m;
    vector<vector<int>> a(n, vector<int>(m, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> a[i][j];
        }
    }
    vector<int> calc(m, 0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (a[i][j] == 1) {
                calc[j] = 0;
            } else {
                calc[j]++;
            }
        }
        maximum = max(square(calc, m), maximum);
    }
    cout << maximum;
}