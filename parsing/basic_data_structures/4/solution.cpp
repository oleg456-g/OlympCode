#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cmath>
#include <string>
typedef long long ll;
ll inf = 1000000000000000000;
using namespace std;
int check(vector <int> a, int n) {
    vector <int> st;
    vector <int> l(n);
    vector <int> r(n);
    for (int i = n - 1; i >= 0; i--) {
        while (!st.empty() && a[i] <= a[st.back()]) {
            st.pop_back();
        }
        if (st.empty()) {
            r[i] = n;
        } else {
            r[i] = st.back();
        }
        st.push_back(i);
    }
    st.clear();
    for (int i = 0; i < n; i++) {
        while (!st.empty() && a[i] <= a[st.back()]) {
            st.pop_back();
        }
        if (st.empty()) {
            l[i] = -1;
        } else {
            l[i] = st.back();
        }
        st.push_back(i);
    }
    int max = 0, s;
    for (int i = 0; i < n; i++) {
        s = a[i] * (r[i] - l[i] - 1);
        if (s > max) {
            max = s;
        }
    }
    return(max);
}
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    vector <vector<int>> a(n, vector<int>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> a[i][j];
        }
    }
    vector <vector<int>> b(n, vector<int>(m));
    for (int i = 0; i < m; i++) {
        if (a[0][i] == 0) {
            b[0][i] = 1;
        } else {
            b[0][i] = 0;
        }
    }
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (a[i][j] == 1) {
                b[i][j] = 0;
            } else {
                b[i][j] = b[i - 1][j] + 1;
            }
        }
    }
    int max = 0, s;
    for (int i = 0; i < n; i++) {
        s = check(b[i], m);
        if (s > max) {
            max = s;
        }
    }
    cout << max;
}