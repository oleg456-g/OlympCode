#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>
#include <cstdio>
#include <iomanip>

using namespace std;

#define all(x) x.begin(), x.end()

using ll = long long;



void solve() {
        int a, b, c, d, e, f;

        cout << 3 << endl;
        cin >> a;
        cout << 4 << endl;
        cin >> b;
        cout << 5 << endl;
        cin >> c;

        cout << 6 << endl;
        cin >> d;

        cout << 2 << endl;
        cin >> e;

        cout << 1 << endl;
        cin >> f;
        if (a == 2 || b == 2 || a == 1 || b == 1 || a == 3) {
                for (int i = 0; i < 499; i++) {
                        cout << 3 << endl;
                        cin >> a;
                        cout << 4 << endl;
                        cin >> a;
                        cout << 5 << endl;
                        cin >> a;

                        cout << 6 << endl;
                        cin >> a;

                        cout << 2 << endl;
                        cin >> a;

                        cout << 1 << endl;
                        cin >> a;
                }
        }
        else {
                for (int i = 0; i < 499; i++) {
                        cout << 1 << endl;
                        cin >> a;
                        cout << 6 << endl;
                        cin >> a;
                        cout << 5 << endl;
                        cin >> a;

                        cout << 4 << endl;
                        cin >> a;

                        cout << 3 << endl;
                        cin >> a;

                        cout << 2 << endl;
                        cin >> a;

                }
        }
}

signed main() {
        //ios_base::sync_with_stdio(false);
        //cin.tie(nullptr);
        cout << fixed << setprecision(9);
#ifndef ONLINE_JUDGE
        //freopen("input.txt", "r", stdin);
        //freopen("output.txt", "w", stdout);
#endif

        int t = 1;
        //cin >> t;
        for (int _ = 0; _ < t; _++)
                solve();

}