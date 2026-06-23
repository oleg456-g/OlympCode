#include "testlib.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <set>
#include <utility>

using namespace std;


int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);
    int t;
    t = inf.readInt();
    long double score = 0;
    for (int testcase = 0; testcase < t; ++testcase) {
        int n = inf.readInt();
        int m = inf.readInt();
        int k = inf.readInt();
        vector<pair<int, int>> bird(k);

        for (int i = 0; i < k; ++i) {
            bird[i].first = inf.readInt();
            bird[i].second = inf.readInt();
        }
        int ans_p = ouf.readInt();
		if (ans_p < 0 || ans_p > k * 10) {
			quitf(_wa, "Wrong number of scarecrows");
		}
        vector<pair<int, int>> sc_p(ans_p);
        vector<vector<bool>> used(n, vector<bool>(n, false));

        for (int i = 0; i < ans_p; ++i) {
            sc_p[i].first = ouf.readInt();
            sc_p[i].second = ouf.readInt();
            if (sc_p[i].first < 1 || sc_p[i].first > n || sc_p[i].second < 1 || sc_p[i].second > n) {
                quitf(_wa, "Incorrect coordinate of the scarecrow %d: (%d, %d)", testcase, sc_p[i].first, sc_p[i].second);
            }
        }
        for (auto el : sc_p) {
            for (int i = max(0, el.first - 1 - m); i <= min(n - 1, el.first - 1 + m); i++) {
                for (int j = max(0, el.second - 1 - m); j <= min(n - 1, el.second - 1 + m); j++) {
                    used[i][j] = true;
                }
            }
        }
        for (auto el : bird) {
            if (!used[el.first - 1][el.second - 1]) {
                quitf(_wa, "The bird is not covered %d: (%d, %d)", testcase, el.first, el.second);
            }
        }

        int ans_j = ans.readInt();
        vector<pair<int, int>> sc_j(ans_j);
        for (int i = 0; i < ans_j; ++i) {
            sc_j[i].first = ans.readInt();
            sc_j[i].second = ans.readInt();
        }

        /*if (ans_p < ans_j) {
            quitf(_fail, "Particiapnt found better solution at testcase %d", testcase);
        }*/
        long double mul = 1.0;
        int deg = 3;
        for (int _ = 0; _ < deg; _++) {
            mul *= (1.0 * ans_j) / ans_p;
        }
        score += 10.0 * min((long double)1.0, mul);
    }
    quitp(score, "participant's solution is ok");
    return 0;
}
