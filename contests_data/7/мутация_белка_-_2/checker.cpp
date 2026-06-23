#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

map<string, char> trans;

const vector <pair <int, char>> G = {
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
const string c = "UCAG";

void precalc() {
    int i = 0;
    auto g = G;
    for (char v1 : c) {
        for (char v2 : c) {
            for (char v3 : c) {
                string s;
                s += v1;
                s += v2;
                s += v3;
                if (g[i].first == 0) i++;
                g[i].first--;
                trans[s] = g[i].second;
            }
        }
    }
}

string build(const string& s, const vector<int>& del) {
    string t, cur, res;
    int id = 0;
    for (int i = 0; i < (int)s.size(); i++) {
        if (id < (int)del.size() && del[id] == i + 1) {
            id++;
            continue;
        }
        cur += s[i];
        if ((int)cur.size() == 3) {
            res += trans[cur];
            cur.clear();
        }
    }
    if (!cur.empty()) return "";
    return res;
}

vector<int> read_answer(InStream& in, int k, int len) {
    vector<int> a;
    for (int i = 0; i < k; i++) {
        a.push_back(in.readInt(1, len, "deleted_index"));
    }
    sort(a.begin(), a.end());
    for (int i = 1; i < k; i++)
        if (a[i] == a[i - 1]) 
            in.quitf(_wa, "indices must be strictly increasing");
    return a;
}

string comment;

double sum_score = 0.;

void solve() {
    int n = inf.readInt();
    inf.readSpace();
    int k = inf.readInt();
    inf.readEoln();
    string s = inf.readToken();
    inf.readEoln();

    int len = (int)s.size();

    vector<int> part_del = read_answer(ouf, k, len);
    string part = build(s, part_del);

    bool st = false;
    bool en = false;
    int cnt = 0;
    for (auto v : part) {
        if (!st && v == 'M') {
            st = true;
            cnt = 0;
        }
        if (st) {
            cnt++;
        }
        if (st && v == 'e') {
            en = true;
            break;
        }
            
    }
    if (!en) {
        cnt = 0;
    }
    double score = 100.0 * (n - abs(cnt - n)) / max(n, cnt);
    sum_score += score;
}

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);
    
    precalc();
    
    int t = inf.readInt();
    inf.readEoln();
    for (int i = 0; i < t; i++) {
        solve();
    }
    sum_score /= t;
    sum_score /= 100.;
    sum_score *= 60.;
    quitp(sum_score, format("points_info=score=%.6f", sum_score).c_str());
}
