#include <bits/stdc++.h>

using namespace std;

#define x first
#define y second

using pi = pair<int, int>;

int dst(pi a, pi b) {
	return abs(a.x - b.x) + abs(a.y - b.y);
}

vector<vector<int>> s;

int main() {
	int t_cnt;
	cin >> t_cnt;
	int n, m, k;
	cin >> n >> m >> k;
	vector<int> p;
	for (int i = 0; i < n; i++) {
		p.emplace_back(i + 1);
	}
	for (int i = 0; i < m; i++) {
		p.emplace_back(-i - 1);
	}

	s.resize(n + 1);
	for (int i = 1; i <= n; i++) {
		s[i].resize(m + 1);
		for (int j = 1; j <= m; j++) {
			cin >> s[i][j];
		}
	}
	sort(p.begin(), p.end());

	int ans = 1e9;
	vector<int> anp = p;

	do {
		vector<pi> pe(n + 1);
		vector<pi> pf(m + 1);
		for (int i = 0; i < k * k; i++) {
			pi pos = {i % k, i / k};
			if (p[i] > 0) {
				pe[p[i]] = pos;
			} else {
				pf[-p[i]] = pos;
			}
		}
		int an = 0;
		for (int i = 1; i <= n; i++) {
			for (int j = 1; j <= m; j++) {
				an += dst(pe[i], pf[j]) * s[i][j];
			}
		}
		if (an < ans) {
			ans = an;
			anp = p;
		}

	} while (next_permutation(p.begin(), p.end()));

	p = anp;
	for (int i = 0; i < k * k; i++) {
		cout << p[i] << ' ';
		if (i % k == k - 1) {
			cout << '\n';
		}
	}
}