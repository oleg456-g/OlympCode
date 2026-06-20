#include <bits/stdc++.h>

#define x first
#define y second

using namespace std;

using ll = long long;
using ull = unsigned long long;

constexpr ll p = 1000'000'007;

mt19937 rnd;

struct hsh {
	ll a;
	ull b;

	hsh(unsigned x) {
		a = x % p;
		b = x;
	}

	hsh(const hsh& a) = default;

	void operator+=(const hsh& x) {
		a += x.a;
		b += x.b;
		if (a >= p) {
			a -= p;
		}
	}

	hsh operator+(const hsh& x) const {
		hsh r(*this);
		r += x;
		return r;
	}

	void operator-=(const hsh& x) {
		a -= x.a;
		b -= x.b;
		if (a < 0) {
			a += p;
		}
	}

	hsh operator-(const hsh& x) const {
		hsh r(*this);
		r -= x;
		return r;
	}

	void operator*=(const hsh& x) {
		a *= x.a;
		b *= x.b;
		a %= p;
	}

	hsh operator*(const hsh& x) const {
		hsh r(*this);
		r *= x;
		return r;
	}

	auto operator<=>(const hsh& x) const {
		return tie(a, b) <=> tie(x.a, x.b);
	}
};

const hsh q(257);

vector<hsh> pw;

void initpw(int n) {
	pw.emplace_back(1);
	for (int i = 1; i <= n; i++) {
		pw.emplace_back(pw[i - 1] * q);
	}
}

struct str {
	string s;
	vector<hsh> h;

	str() {
		h.emplace_back(0);
	}

	void add(char x) {
		s += x;
		h.emplace_back(h.back() * q + hsh(x));
		}

	hsh substr(int l, int r) {
		return h[r] - h[l] * pw[r - l];
	}

	int calck(int k) {
		int res = 0;
		set<hsh> used;
		for (int i = 0; i + k <= s.size(); i++) {
			hsh hs = substr(i, i + k);
			if (!used.count(hs)) {
				res++;
				used.insert(hs);
			}
		}
		return res;
	}
};

vector<pair<int, char>> t;

string gen_random() {
	string s;
	for (auto i : t) {
		for (int j = 0; j < i.x; j++) {
			s += i.y;
		}
	}
	shuffle(s.begin(), s.end(), rnd);
	return s;
}

string gen_greedy(int n, int k, bool shuf) {
	str s;
	map<hsh, int> used;
	auto tt = t;
	
	for (int i = 1; i <= n; i++) {
		if (i < k) {
			for (auto& j : tt) {
				if (j.x == 0) {
					continue;
				}
				s.add(j.y);
				j.x--;
				break;
			}
			continue;
		}
		hsh h = s.substr(i - k, i - 1);
		h *= q;

		if (shuf) {
			shuffle(tt.begin(), tt.end(), rnd);
		}

		int v = -1;
		int vc = 1e9;
		for (int j = 0; j < tt.size(); j++) {
			if (tt[j].x == 0) {
				continue;
			}
			if (used[h + tt[j].y] < vc) {
				vc = used[h + tt[j].y];
				v = j;
			}
		}
		s.add(tt[v].y);
		tt[v].x--;
		used[h + tt[v].y]++;
	}

	return s.s;
}

struct de_bruijn {
	vector<int> dba;
	vector<int> db;
	int dbn, dbk;
	string s;

	void ddb(int t, int p) {
	    if (t > dbn) {
	        if (dbn % p == 0) {
	            for (int j = 1; j < p + 1; j++) {
	                db.push_back(dba[j]);
	            }
	        }
	    } else {
	        dba[t] = dba[t - p];
	        ddb(t + 1, p);
	        for (int j = dba[t - p] + 1; j < dbk; j++) {
	            dba[t] = j;
	            ddb(t + 1, t);
	        }
	    }
	}

	de_bruijn(int n, int k, int m) {
		dbk = m;
		while (pow(dbk - 1, k) > n - k) {
			dbk--;
		}
		dbn = k;
        dba.clear();
        dba.resize(dbn * dbk);
        db.clear();
        ddb(1, 1);
        for (int i = 0; i < min(n, (int)db.size()); i++) {
            s += t[db[i]].y;
        }
        for (int i = db.size(); i < n; i++) {
            s += s[i - db.size()];
        }
        auto tt = t;
        for (int i = 0; i < s.size(); i++) {
        	for (int j = 0; j < tt.size(); j++) {
        		if (t[j].y != s[i]) {
        			continue; 
        		}
        		if (t[j].x > 0) {
        			t[j].x--;
        		} else {
        			s[i] = 0;
        		}
        		break;
        	}
        }

        for (int i = 0; i < s.size(); i++) {
        	if (s[i] != 0) {
        		continue;
        	}
        	for (int j = 0; j < tt.size(); j++) {
        		if (t[j].x > 0) {
        			s[i] = t[j].y;
        			t[j].x--;
        			break;
        		}
        	}
        }
	}
};

int check(string s, int k) {
	str ss;
	for (auto i : s) {
		ss.add(i);
	}
	return ss.calck(k);
}

int main() {
	int testcnt;
	cin >> testcnt;
	int m, k, n = 0;
	cin >> m >> k;
	t.resize(m);
	string s;
	for (int i = 0; i < m; i++) {
		cin >> t[i].y >> t[i].x;
		n += t[i].x;
		for (int j = 0; j < t[i].x; j++) {
			s += t[i].y;
		}
	}
	initpw(n);
	sort(s.begin(), s.end());
	string ans = s;
	int an = 0;
	do {
		int a = check(s, k);
		if (a > an) {
			an = a;
			ans = s;
		}
	} while (next_permutation(s.begin(), s.end()));
	cout << ans << '\n';
}

