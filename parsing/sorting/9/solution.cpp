#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

int main() {
    int n;
    char reserv;
    cin >> n;
    vector<char> s(n, 'a');
    vector<char> ans(n, 'a');
    for (int i = 0; i < n; i++) {
        cin >> s[i];
    }
    sort(s.begin(), s.end());
    reserv = s[n - 1];
    int i, p;
    i = 0;
    p = 0;
    while (i < n - 1) {
        if (s[i] == s[i + 1]) {
            ans[n - p - 1] = s[i];
            ans[p] = s[i];
            p++;
            i += 2;
        } else {
            if (reserv == s[n - 1]) {
                reserv = s[i];
            }
            i++;
        }
    }
    bool flag;
    flag = true;
    for (int i = 0; i < n; i++) {
        if (flag == true && ans[i] == 'a') {
            flag = false;
            cout << reserv;
        } else if (ans[i] != 'a') {
            cout << ans[i];
        }
    }
}