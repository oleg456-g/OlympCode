#include "testlib.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <set>
#include <utility>

using namespace std;

long double count_profit(vector <long long>& x, vector <long long>& c, vector <int>& v) {
    long double profit = 1;
    long double cost = 0;
    for (int i = 0; i < v.size(); i++) {
        profit *= ((long double)1.0 + x[v[i] - 1] / 100.0);
        cost += c[v[i] - 1];
    }
    return profit - cost;
}

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);
    int t;
    t = inf.readInt();
    long double score = 0;
    for (int testcase = 0; testcase < t; ++testcase) {
        int n = inf.readInt();
        vector <long long> x(n);
        vector <long long> c(n);

        for (int i = 0; i < n; ++i) {
            x[i] = inf.readInt();
        }
        for (int i = 0; i < n; ++i) {
            c[i] = inf.readInt();
        }
        int n_p = ouf.readInt();
		if (n_p < 0 || n_p > n) {                
			quitf(_wa, "Wrong count numbers in %d: %d", testcase, n_p);
		}
        vector <int> out_p(n_p);
        vector<bool> used(n + 1, false);
        for (int i = 0; i < n_p; ++i) {
            out_p[i] = ouf.readInt();
            if (out_p[i] > n || out_p[i] < 1) {
                quitf(_wa, "Deprecated number in testcase %d: %d", testcase, out_p[i]);
            }
            if (used[out_p[i]]) {
                quitf(_wa, "Doubly number in testcase %d: %d", testcase, out_p[i]);
            }
            used[out_p[i]] = true;
        }

        int n_j = ans.readInt();
        vector <int> ans_j(n_j);
        for (int i = 0; i < n_j; ++i) {
            ans_j[i] = ans.readInt();
        }

        int pa = count_profit(x, c, out_p);
        int ja = count_profit(x, c, ans_j);

        /*if (pa < ja) {
            quitf(_fail, "Particiapnt found better solution at testcase %d", testcase);
        }*/
        if (pa > 0) {
            long double mul = 1.0;
            int deg = 10;
            for (int _ = 0; _ < deg; _++) {
                mul *= (1.0 * pa) / ja;
            }
            score += 10.0 * mul;
        }
    }
    quitp(score, "participant's solution is ok");
    return 0;
}
