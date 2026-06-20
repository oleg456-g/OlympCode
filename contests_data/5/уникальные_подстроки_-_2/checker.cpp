#include <bits/stdc++.h>
#include "testlib.h"

using namespace std;

int readAns(InStream& in, int k, vector<pair<int, char>> t) {
    int n = 0;
    string let;
    for (auto i : t) {
        n += i.first;
        let += i.second;
    }
    string s = in.readToken("[" + let + "]{" + to_string(n) + "}");
    for (auto i : s) {
        bool found = false;
        for (int j = 0; j < t.size(); j++) {
            if (i == t[j].second) {
                if (t[j].first == 0) {
                    in.quitf(_wa, "Letter %c used too much", t[j].second);
                }
                t[j].first--;
                found = true;
            }
        }
        if (!found) {
            in.quitf(_wa, "Unknown letter %c", i);
        }
    }

    unordered_set<string> used;
    for (int i = 0; i + k <= s.size(); i++) {
        used.insert(s.substr(i, k));
    }
    return used.size();
}

int main(int argc, char *argv[]) {
    registerTestlibCmd(argc, argv);
	int testcnt = inf.readInt();
	double score = 0;
	for (int testno = 0; testno < testcnt; ++testno) {
		int m = inf.readInt();
		int k = inf.readInt();
		vector<pair<int, char>> t(m);
		for (int i = 0; i < m; i++) {
			t[i].second = inf.readToken()[0];
			t[i].first = inf.readInt();
		}

		int pa = readAns(ouf, k, t);
		int ja = readAns(ans, k, t);
		if (pa > ja) {
            quitf(_fail, "Better than jury at test %d", testno);
        }
		double sc = 5 * pow(1.0 * pa / ja, 5);
		score += sc;
	}
    quitp(score, "");
}
