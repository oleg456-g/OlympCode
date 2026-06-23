#include <bits/stdc++.h>

using namespace std;

map<string, char> trans;
map<char, vector <string>> retrans;

const int MAXN = 4000;
const int MAXK = 1000;
const int MAXC = 10;

tuple<int, int> dp[MAXN][MAXK + 1][MAXC];
bool used[MAXN][MAXK + 1][MAXC];

void pricalk() {
    vector <pair <int, char>> g = {
            {2, 'F'}, {2, 'L'},
            {4, 'S'},
            {2, 'Y'}, {2, 'e'},
            {2, 'C'}, {1, 'e'}, {1, 'W'},
            {4, 'L'},
            {4, 'P'},
            {2, 'H'}, {2, 'Q'},
            {4, 'R'},
            {3, 'I'}, {1, 'M'},
            {4, 'T'},
            {2, 'N'}, {2, 'K'},
            {2, 'S'}, {2, 'R'},
            {4, 'V'},
            {4, 'A'},
            {2, 'D'}, {2, 'E'},
            {4, 'G'}
    };

    vector <char> c = {'U', 'C', 'A', 'G'};

    int i = 0;
    for (auto v1 : c) {
        for (auto v2 : c) {
            for (auto v3 : c) {
                string s = string(1, v1);
                s += v2;
                s += v3;
                if (g[i].first == 0) {
                    i++;
                }
                g[i].first--;
                trans[s] = g[i].second;
                retrans[g[i].second].push_back(s);
            }
        }
    }
}

void solve() {
    int n, k;
    cin >> n >> k;
    string s;
    cin >> s;
    n = 3 * n + k;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= k; j++) {
            for (int t = 0; t < 10; t++) {
                used[i][j][t] = false;
            }
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= min(i + 1, k); j++) {
            for (int t = 0; t < 10; t++) {
                if (i > 0 && j > 0) {
                    dp[i][j][t] = {j - 1, t};
                    used[i][j][t] = used[i - 1][j - 1][t];
                }
            }
            if (i == 0 && j == 1) {
                dp[0][1][0] = {0, 0};
                used[0][1][0] = true;
            }
            // state 0
            if ((i == 0 && j == 0) || (i > 0 && used[i - 1][j][0])) {
                if (s[i] == 'A') {
                    dp[i][j][1] = {j, 0};
                    used[i][j][1] = true;
                }
            }
            // state 1
            if (i > 0 && used[i - 1][j][1]) {
                if (s[i] == 'U') {
                    dp[i][j][2] = {j, 1};
                    used[i][j][2] = true;
                }
            }
            // state 2
            if (i > 0 && used[i - 1][j][2]) {
                if (s[i] == 'G') {
                    dp[i][j][3] = {j, 2};
                    used[i][j][3] = true;
                }
            }
            // state 3
            if (i > 0 && used[i - 1][j][3]) {
                if (s[i] == 'U') {
                    dp[i][j][4] = {j, 3};
                    used[i][j][4] = true;
                } else {
                    dp[i][j][8] = {j, 3};
                    used[i][j][8] = true;
                }
            }
            // state 4
            if (i > 0 && used[i - 1][j][4]) {
                if (s[i] == 'A') {
                    dp[i][j][5] = {j, 4};
                    used[i][j][5] = true;
                } else if (s[i] == 'G') {
                    dp[i][j][6] = {j, 4};
                    used[i][j][6] = true;
                } else {
                    dp[i][j][9] = {j, 4};
                    used[i][j][9] = true;
                }
            }
            // state 5
            if (i > 0 && used[i - 1][j][5]) {
                if (s[i] == 'A' || s[i] == 'G') {
                    dp[i][j][7] = {j, 5};
                    used[i][j][7] = true;
                } else {
                    dp[i][j][3] = {j, 5};
                    used[i][j][3] = true;
                }
            }
            // state 6
            if (i > 0 && used[i - 1][j][6]) {
                if (s[i] == 'A') {
                    dp[i][j][7] = {j, 6};
                    used[i][j][7] = true;
                } else {
                    dp[i][j][3] = {j, 6};
                    used[i][j][3] = true;
                }
            }
            // state 8
            if (i > 0 && used[i - 1][j][8]) {
                dp[i][j][9] = {j, 8};
                used[i][j][9] = true;
            }
            // state 9
            if (i > 0 && used[i - 1][j][9]) {
                dp[i][j][3] = {j, 9};
                used[i][j][3] = true;
            }
        }
    }
    vector <int> ans;
    int i = n - 1, j = k, st = 7;
    while (j != 0) {
        auto [dj, dst] = dp[i][j][st];
        if (dj != j) {
            ans.push_back(i + 1);
        }
        i--;
        j = dj;
        st = dst;
    }
    sort(ans.begin(), ans.end());
    for (auto v : ans) {
        cout << v << " ";
    }
    cout << "\n";
}

int main() {
    pricalk();
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
    
}
