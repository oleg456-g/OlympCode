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

string s;

vector <int> find_end(string t) {
    vector <int> ans;
    int state = 0;
    for (int i = (int)s.size() - 1; i >= 0; i--) {
        if (s[i] == t[state]) {
            state++;
            if (state == 3) {
                break;
            }
        } else {
            ans.push_back(i + 1);
        }
    }
    return ans;
}

bool is_stop(string t) {
    return t == "UAA" || t == "UAG" || t == "UGA";

    
}
vector <bool> used;
vector <int> ans;
vector <int> ans2, ans3;


void solve() {
    int n, k;
    cin >> n >> k;
    cin >> s;
    n = 3 * n + k;
    used.clear();
    ans.clear();
    ans2.clear();
    ans3.clear();
    used.resize(n, false);
    int state = 0;
    for (int i = 0; i < n; i++) {
        if (state == 0 && s[i] == 'A') {
            state = 1;
        } else if (state == 1 && s[i] == 'U') {
            state = 2;
        } else if (state == 2 && s[i] == 'G') {
            state = 3;
            break;
        } else {
            ans.push_back(i + 1);
        }
    }
    int start = ans.size() + 3;
    ans2 = find_end("AAU");
    ans3 = find_end("GAU");
    if (ans2.size() > ans3.size()) {
        swap(ans2, ans3);
    }
    ans3 = find_end("AGU");
    if (ans2.size() > ans3.size()) {
        swap(ans2, ans3);
    }
    int end = n - (int)ans2.size() - 3;
    for (auto v : ans2) {
        ans.push_back(v);
    }
    k -= ans.size();
    for (int i = start; i + 2 < end && k > 0; i += 3) {

        string t;
        t += s[i];
        t += s[i + 1];
        t += s[i + 2];
        if (is_stop(t)) {
            ans.push_back(i + 1);
            used[i] = true;
            i++;
            k--;
        }
    }
    for (int i = end - 1; i >= start && k > 0; i--) {
        if (!used[i]) {
            used[i] = true;
            k--;
            ans.push_back(i + 1);
        }
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
