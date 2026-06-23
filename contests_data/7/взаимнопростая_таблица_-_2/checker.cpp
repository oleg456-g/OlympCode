#include "testlib.h"

#include <bits/stdc++.h>

using namespace std;

static int gcd_int(int a, int b) {
    while (b != 0) {
        int t = a % b;
        a = b;
        b = t;
    }
    return a;
}

pair<double, int> score() {
    int n = inf.readInt();
    int m = n * n;

    vector<vector<int>> a(n, vector<int>(n));
    vector<int> used(m + 1, 0);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            string name = format("table[%d][%d]", i + 1, j + 1);
            int x = ouf.readInt(1, m, name.c_str());

            if (used[x]) {
                quitf(_wa, "number %d is used more than once", x);
            }
            used[x] = 1;
            a[i][j] = x;
        }
    }

    for (int x = 1; x <= m; ++x) {
        if (!used[x]) {
            quitf(_wa, "number %d is missing", x);
        }
    }

    int bad = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i + 1 < n && gcd_int(a[i][j], a[i + 1][j]) > 1) {
                ++bad;
            }
            if (j + 1 < n && gcd_int(a[i][j], a[i][j + 1]) > 1) {
                ++bad;
            }
        }
    }

    double percent = max(0.0, 100.0 * (1 - 1.7 * (double)bad / (n * n)));
    
    return make_pair(percent, bad);
}

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    int t = inf.readInt();
    double sum_score = 0;
    string bad = "";
    
    
    for (int i = 1; i <= t; ++i) {
	    setTestCase(i);
        pair<double, int> data = score();
        sum_score += data.first;
        if (bad.empty())
            bad += to_string(data.second);
        else
            bad += ", " + to_string(data.second);
    }
    
    if (!ouf.seekEof()) {
        quitf(_pe, "extra data after the last table");
    }

    double total_score = sum_score / t;
    
    quitp(total_score, "bad pairs = %s, percent = %.12f", bad.c_str(), total_score);
}