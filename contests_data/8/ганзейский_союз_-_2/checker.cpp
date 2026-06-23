#include "testlib.h"
#include <vector>
#include <iostream>

#define ll long long

using namespace std;

ll readAns(InStream& stream, int n, int s, int k, const vector<vector<ll>>& c, const vector<vector<ll>>& w) {
	int x = stream.readInt(-1, s);
	if (x == -1) {
		return 0;
	} else if (x < 2) {
		quitf(_wa, "bad x value, should be -1 or >= 2");
	}
	ll ans = stream.readLong(0ll, 1e18);
	vector<int> path(x);
	vector<int> goods(x - 1);
	path[0] = stream.readInt(1, 1) - 1;
	for (int i = 1; i < x; ++i) {
		path[i] = stream.readInt(1, n) - 1;
	}
	for (int i = 0; i < x - 1; ++i) {
		goods[i] = stream.readInt(1, k) - 1;
	}
	ll res = 0;
	for (int i = 1; i < x; ++i) {
		res += c[goods[i - 1]][path[i]] - c[goods[i - 1]][path[i - 1]] - w[path[i - 1]][path[i]];
	}
	if (ans != res) {
		quitf(_wa, "answer does not match with given path");
	}
	return ans;
}

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);
	int score = 0;
	bool flag = true;
	int t = inf.readInt();
	for (int testno = 0; testno < t; ++testno) {
		int n = inf.readInt(2, 300);
		int s = inf.readInt(2, 200);
		int k = inf.readInt(1, 1000);
		vector<vector<ll>> c(k, vector<ll>(n));
		for (int i = 0; i < k; ++i) {
			for (int j = 0; j < n; ++j) {
				c[i][j] = inf.readInt(1, 1e9);
			}
		}
		vector<vector<ll>> w(n, vector<ll>(n));
		for (int i = 1; i < n; ++i) {
			for (int j = 0; j < i; ++j) {
				w[i][j] = w[j][i] = inf.readInt(0, 1e9);
			}
		}
		ll jans = readAns(ans, n, s, k, c, w);
		ll pans = readAns(ouf, n, s, k, c, w);
		if (jans > pans) {
			flag = false;
		} else if (jans == pans) {
			score += 5;
		} else {
			quitf(_fail, ":( participant has the better answer: jans = %d, pans = %d\n", jans, pans);
		}
	}
    if (flag) {
		quitp(score, "participant's solution is ok");
	} else if (score) {
		quitp(score, "participant's solution is partially correct");
	} else {
		quitf(_wa, "participant's solution is wrong");
	}
    return 0;
}