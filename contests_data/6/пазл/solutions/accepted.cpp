#include<bits/stdc++.h>
using namespace std;
#define pb  push_back
#define ll  long long
#define vi  vector<ll >
#define vvi vector<vi >
#define all(x) x.begin(), x.end()

ll n, m, k;
vector<string> a;
vvi b;

void go(ll i, ll j, ll e) {
    if (e == 0) {
        if (i + 1 >= n || j + 1 >= m) return;
        if (a[i][j+1] == '.' && a[i+1][j] == '.' && a[i+1][j+1] == '.') {
            a[i][j] = a[i][j+1] = a[i+1][j] = a[i+1][j+1] = '#';
            b[i][j] = b[i][j+1] = b[i+1][j] = b[i+1][j+1] = k; ++k;
        }
        return;
    }
    if (e == 1) {
        if (i + 2 >= n || j + 2 >= m) return;
        if (a[i  ][j] != '.' || a[i  ][j+1] != '.' || a[i  ][j+2] != '#') return;
        if (a[i+1][j] != '.' || a[i+1][j+1] != '#' || a[i+1][j+2] != '.') return;
        if (                    a[i+2][j+1] != '.' || a[i+2][j+2] != '.') return;
        a[i  ][j  ] = a[i  ][j+1] = '#';
        a[i+1][j  ] = a[i+1][j+2] = '#';
        a[i+2][j+1] = a[i+2][j+2] = '#';
        b[i  ][j  ] = b[i  ][j+1] = k;
        b[i+1][j  ] = b[i+1][j+2] = k;
        b[i+2][j+1] = b[i+2][j+2] = k; ++k;
        return;
    }
    if (e == 2) { --j;
        if (i + 2 >= n || j + 2 >= m) return;
        if (a[i  ][j] != '#' || a[i  ][j+1] != '.' || a[i  ][j+2] != '.') return;
        if (a[i+1][j] != '.' || a[i+1][j+1] != '#' || a[i+1][j+2] != '.') return;
        if (a[i+2][j] != '.' || a[i+2][j+1] != '.'                      ) return;
        a[i  ][j+1] = a[i  ][j+2] = '#';
        a[i+1][j  ] = a[i+1][j+2] = '#';
        a[i+2][j  ] = a[i+2][j+1] = '#';
        b[i  ][j+1] = b[i  ][j+2] = k;
        b[i+1][j  ] = b[i+1][j+2] = k;
        b[i+2][j  ] = b[i+2][j+1] = k; ++k;
        return;
    }
}

bool solve() {
    cin >> n >> m; k = 1;
    a.resize(n);
    for (ll i = 0; i < n; ++i)
        cin >> a[i];
    b.assign(n, vi(m, 0));
    for (ll i = 0; i < n; ++i) {
        for (ll j = 0; j < m; ++j) {
            if (a[i][j] == '#') continue;
            for (ll e = 0; e < 3; ++e)
                go(i, j, e);
            if (a[i][j] == '.') return false;
        }
    }
    return true;
}

int main() {
    ios_base::sync_with_stdio(false); cin.tie(0);
    int t; cin >> t;
    while (t--) {
        if (solve()) {
            cout << "Yes\n";
            for (ll i = 0; i < n; ++i) {
                for (ll j = 0; j < m; ++j)
                    cout << b[i][j] << " ";
                cout << "\n";
            }
        }
        else
            cout << "No\n";
    }

    return 0;
}
