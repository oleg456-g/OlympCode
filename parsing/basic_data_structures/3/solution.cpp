#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stack>


using namespace std;


int main() {
    long int n;
    long long maximum, temp;
    cin >> n;
    vector<long long> a(n, 0);
    vector<long long> res1(n, 0);
    stack<long long> st1;
    vector<long long> res2(n, 0);
    stack<long long> st2;
    maximum = 0;
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
    cout << maximum;
}