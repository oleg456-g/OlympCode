#include <bits/stdc++.h>

using namespace std;

map<string, char> trans;
map<char, vector <string>> retrans;

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

vector <int> p;
string s;
int n, k;
string ans;
bool ok = false;

void resh(int v, int k_) {
    if (k_ == -1) {
        ans.clear();
        int id = 0;
        string str;
        for (int i = 0; i < s.size(); i++) {
            if (id != p.size() && i == p[id]) {
                id++;
                continue;
            }

            str += s[i];
            if (str.size() == 3) {
                ans += trans[str];
                str.clear();
            }
        }
        if (ans[0] != 'M' || ans.back() != 'e') {
            return;
        }
        for (int i = 1; i + 1 < ans.size(); i++) {
            if (ans[i] == 'e') {
                return;
            }
        }
        ok = true;
        return;
    }
    while (v >= 0) {
        p[k_] = v;
        resh(v - 1, k_ - 1);
        if (ok) {
            return;
        }
        v--;
    }
}

void solve() {
    pricalk();
    cin >> n >> k;
    cin >> s;
    p.resize(k, 0);
    resh(3 * n + k - 1, k - 1);
    for (auto v : p) {
        cout << v + 1 << " ";
    }
    cout << "\n";
}

int main() {
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}
