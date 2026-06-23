#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <cmath>
#include <map>
#include <deque>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <numeric>
#include <random>
#include <stack>
#include <bitset>
#include <unordered_map>


typedef long long ll;


using namespace std;

const int MAX_N = 1e5 + 5, MOD = 1e9 + 7, E = 26, EDGES = 8;

int cnt[E][E], palindrome[E], pw[EDGES + 1], placements[E][E][EDGES + 1], place_pal[E][EDGES + 1];


int cur[E][E], used[E][E], pascal[EDGES + 1][EDGES + 1];

vector <pair<int, int> > uniques;

void precalc() {
    uniques.reserve(EDGES);
    pw[0] = 1; 
    for (int i = 1; i <= EDGES; i++) {
        pw[i] = 2 * pw[i - 1];
    }

    for (int i = 0; i <= EDGES; i++) {
        pascal[i][0] = 1;
        pascal[i][i] = 1;
        for (int j = 1; j < i; j++) {
            pascal[i][j] = pascal[i - 1][j] + pascal[i - 1][j - 1];
        }
    }

    for (int a = 0; a < E; a++) {
        for (int b = a; b < E; b++) {
            placements[a][b][0] = 1;
            for (int i = 1; i <= EDGES; i++) {
                placements[a][b][i] = 1ll * placements[a][b][i - 1] * (cnt[a][b] - i + 1) % MOD;
            }
        }
    }
    for (int a = 0; a < E; a++) {
        place_pal[a][0] = 1;
        for (int i = 1; i <= EDGES; i++) {
            place_pal[a][i] = 1ll * place_pal[a][i - 1] * (palindrome[a] - i + 1) % MOD;
        }
    }
}

void add(pair<int, int>& p) {
    if (p.first > p.second) {
        swap(p.first, p.second);
    }
    cur[p.first][p.second]++;
    if (!used[p.first][p.second]) {
        uniques.emplace_back(p);
        used[p.first][p.second] = 1;
    }
}

void rm(pair<int, int>& p) {
    cur[p.first][p.second] = 0;
    used[p.first][p.second] = 0;
}





signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    int n, L, ans = 0;
    cin >> n >> L;
    
    for (int i = 0; i < n; i++) {
        string s;
        cin >> s;
        string rs = s;
        reverse(rs.begin(), rs.end());
        if (rs < s) {
            swap(s, rs);
        }
        if (s == rs) {
            palindrome[s[0] - 'a']++;
        } else {
            cnt[s[0] - 'a'][s[L - 1] - 'a']++;
        }
    }

    precalc();

    for (int u = 0; u < E; u++) {
        for (int a = 0; a < E; a++) {
            for (int b = 0; b < E; b++) {
                for (int c = 0; c < E; c++) {
                    for (int d = 0; d < E; d++) {
                        vector <pair <int, int> > edges = {{u, a}, {u, b}, {u, c}, {u, d}, {a, b}, {b, c}, {c, d}, {d, a}};
                        for (auto & e : edges) {
                            add(e);
                        }

                        ll num = 1;

                        for (auto & [x, y] : uniques) {
                            if (x == y) {
                                ll cnt = 0;
                                for (int i = 0; i <= min(cur[x][y], palindrome[x]); i++) {
                                    cnt = (cnt + 1ll * place_pal[x][i] * placements[x][x][cur[x][x] - i] % MOD * pw[cur[x][x] - i] * pascal[cur[x][x]][i]) % MOD;
                                }
                                num = (num * cnt) % MOD;
                            } else {
                                num = (num * placements[x][y][cur[x][y]]) % MOD;
                            }
                            if (!num) {
                                break;
                            }
                        }
                        ans = (ans + num) % MOD;


                        for (auto & e : uniques) {
                            rm(e);
                        }
                        uniques.clear();
                    }
                }
            }
        }
    }
    cout << ans;
}