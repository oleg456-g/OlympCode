#include "testlib.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <set>

using namespace std;

long long recalc(vector <vector <int>> f, vector <vector <int>> p) {
	vector <pair <int, int>> pos_att(p[0].size());
	for (int i = 0; i < f.size(); ++i) {
		for (int j = 0; j < f[0].size(); ++j) {
			if (f[i][j] < 0) {
				int param_num = -f[i][j] - 1;
				pos_att[param_num] = make_pair(i, j);
			}
		}
	}
	long long total_sum = 0;
	for (int i = 0; i < f.size(); ++i) {
		for (int j = 0; j < f[0].size(); ++j) {
			if (f[i][j] > 0) {
				for (int param_num = 0; param_num < p[0].size(); ++param_num) {
					long long dist_i = abs(i - pos_att[param_num].first);
					long long dist_j = abs(j - pos_att[param_num].second);
					total_sum += (dist_i + dist_j) * p[f[i][j] - 1][param_num];

				}
			}
		}
	}
	return total_sum;
}

int main(int argc, char* argv[]) {
	registerTestlibCmd(argc, argv);
	double score = 0;
	int tests = inf.readInt();
	for (int testno = 0; testno < tests; ++testno) {
		int n = inf.readInt();
		int k = inf.readInt();
		int s = inf.readInt();
		vector <vector <int>> p(n);
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < k; ++j) {
				p[i].push_back(inf.readInt());
			}
		}
		vector <vector <int>> jans(s);
		vector <vector <int>> pans(s);
		set <int> used;
		for (int i = 0; i < s; ++i) {
			for (int j = 0; j < s; ++j) {
				int tmp = ouf.readInt();
				if (tmp < -k || tmp > n || tmp == 0) {
					quitf(_wa, "Wrong number %d", tmp);
				}
				if (used.find(tmp) != used.end()) {
					quitf(_wa, "Duplicated number %d", tmp);
				}
				used.insert(tmp);
				pans[i].push_back(tmp);
				tmp = ans.readInt();
				jans[i].push_back(tmp);
			}
		}
		long long pscore = recalc(pans, p);
		long long jscore = recalc(jans, p);
		score += (pow(1.0 * jscore / pscore, 5)) * 5;
	}
	quitp(score, "participant's solution is ok");
	return 0;
}