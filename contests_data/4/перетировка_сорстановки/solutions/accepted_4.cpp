#include <algorithm>
#include <bits/stdc++.h>
#include <queue>

using namespace std;
using ll = long long;
using pi = pair<int, int>;

template<typename T>
bool ckmin(T& a, const T& b) { return b < a ? a = b, true : false; }
template<typename T>
bool ckmax(T& a, const T& b) { return a < b ? a = b, true : false; }

template<typename T>
using pq_min = priority_queue<T, vector<T>, greater<T>>;
template<typename T>
using pq_max = priority_queue<T, vector<T>, less<T>>;

void op(vector<int>& p, int l, int r, vector<pi>& moves) {
    auto it1 = max_element(p.begin()+l, p.begin()+r);
    auto it2 = min_element(p.begin()+l, p.begin()+r);
    swap(*it1, *it2);
}

template<typename T>
void op(T& p, int l, int r) {
    auto it1 = max_element(p.begin()+l, p.begin()+r);
    auto it2 = min_element(p.begin()+l, p.begin()+r);
    swap(*it1, *it2);
}

bool solve(vector<int> p, int k, vector<pi>& moves) {
    int n = p.size();

    if (is_sorted(p.begin(), p.end())) return true;
    if (p.size() == k) {
        op(p, 0, k, moves);
        return is_sorted(p.begin(), p.end());
    }

    if (k == 2) {
        for (int i = 0; i < n; ++i) {
            for (int j = n-1; j > i; --j) {
                if (p[j-1] > p[j]) {
                    moves.emplace_back(j-1, j+1);
                    swap(p[j-1], p[j]);
                }
            }
        }
        return true;
    } else if (k == 3) {
        if (n == 4) {
            for (int i = 0; i < 30; ++i) {
                op(p, i%2, i%2+3, moves);
                if (is_sorted(p.begin(), p.end())) return true;
            }
        } else {
            using a5 = array<int, 5>;
            static map<a5, int> way;
            if (way.empty()) {
                a5 start;
                iota(start.begin(), start.end(), 1);
                way[start] = -1;

                queue<a5> q;
                q.push(start);

                while (!q.empty()) {
                    auto v = q.front(); q.pop();
                    for (int i = 0; i < 3; ++i) {
                        op(v, i, i+3);
                        if (way.try_emplace(v, i).second)
                            q.push(v);
                        op(v, i, i+3);
                    }
                }
            }

            for (int i = 0; i < n; ++i) {
                for (int j = n-5; j >= i; --j) {
                    a5 ord;
                    fill(ord.begin(), ord.end(), 0);
                    for (int x = 0; x < 5; ++x)
                        for (int y = 0; y < 5; ++y)
                            if (p[j+x] <= p[j+y]) ++ord[y];
                    while (!is_sorted(ord.begin(), ord.end())) {
                        const int t = way[ord];
                        op(ord, t, t+3);
                        op(p, j+t, j+t+3, moves);
                        moves.emplace_back(j+t, j+t+3);
                    }
                }
            }

            return true;
        }
    } else if (k == 4) {
        if (n == 5) {
            auto w = p;
            for (int t = 0; ; ++t) {
                op(p, t%2, t%2+4);
                moves.emplace_back(t%2, t%2+4);
                if (is_sorted(p.begin(), p.end())) return true;
                if (p == w) return false;
            }
        } else if (n == 6) {
            using a6 = array<int, 6>;
            static map<a6, int> way;
            if (way.empty()) {
                map<a6, int> dist;
                a6 start;
                iota(start.begin(), start.end(), 1);
                way[start] = -1;

                queue<a6> q;
                q.push(start);

                while (!q.empty()) {
                    auto v = q.front(); q.pop();
                    const int d = dist[v];
                    for (int i = 0; i < 3; ++i) {
                        op(v, i, i+4);
                        if (way.try_emplace(v, i).second)
                            q.push(v), dist[v] =d + 1;
                        op(v, i, i+4);
                    }
                }
            }

            a6 v;
            for (int i = 0; i < 6; ++i) v[i] = p[i];
            if (way.count(v)) {
                while (!is_sorted(v.begin(), v.end())) {
                    const int i = way[v];
                    op(v, i, i+4);
                    moves.emplace_back(i, i+4);
                }
                return true;
            } else {
                return false;
            }
        } else {
            using a7 = array<int, 7>;
            static map<a7, int> way;
            if (way.empty()) {
                map<a7, int> dist;
                a7 start;
                iota(start.begin(), start.end(), 1);
                way[start] = -1;

                queue<a7> q;
                q.push(start);

                int max_d = 0;
                while (!q.empty()) {
                    auto v = q.front(); q.pop();
                    const int d = dist[v]; ckmax(max_d, d);
                    for (int i = 0; i < 4; ++i) {
                        op(v, i, i+4);
                        if (way.try_emplace(v, i).second)
                            q.push(v), dist[v] =d + 1;
                        op(v, i, i+4);
                    }
                }
                // cout << "max_d = " << max_d << endl;
                // cout << "sz " << way.size() << " --> " << 7*6*5*4*3*2 << endl;
            }

            for (int i = 0; i < n; ++i) {
                for (int j = n-7; j >= i; --j) {
                    // cout << "sort " << j << ".." << j+7 << endl;
                    a7 ord;
                    fill(ord.begin(), ord.end(), 0);
                    for (int x = 0; x < 7; ++x)
                        for (int y = 0; y < 7; ++y)
                            if (p[j+x] <= p[j+y]) ++ord[y];
                    // cout << "try to sort ";
                    // for (int x : ord) cout << x;
                    // cout << endl;

                    while (!is_sorted(ord.begin(), ord.end())) {
                        const int t = way[ord];
                        // cout << "t=" << t << ": ";
                        // for (int x: ord) cout << x << " ";
                        // cout << endl;
                        op(ord, t, t+4);
                        op(p, j+t, j+t+4, moves);
                        moves.emplace_back(j+t, j+t+4);
                    }
                }
            }

            return true;
        }
    }

    return false;
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0);

    // vector<int> p(6);
    // iota(p.begin(), p.end(), 1);
    // do {
    //     vector<pi> moves;
    //     bool ans = solve(p, 4, moves);
    //     if (!ans) {
    //         for (int x : p) cout << x << ' ';
    //         cout << endl;
    //     }
    // } while(next_permutation(p.begin(), p.end()));

    int t = 1;
    cin >> t;
    while (t--) {
        int n, k; cin >> n >> k;
        vector<int> p(n);
        for (int& x : p) cin >> x;

        vector<pi> res;
        bool ans = solve(p, k, res);

        if (ans) {
            cout << "Yes\n";
            cout << res.size() << '\n';


            for (auto [l, r] : res) {
                swap(*min_element(p.begin()+l, p.begin()+r),
                     *max_element(p.begin()+l, p.begin()+r));
                if (r - l != k) cerr << "incorrect segment!" << endl;
                cout << l+1 << ' ' << r << '\n';
            }

            if (!is_sorted(p.begin(), p.end())) {
                cerr << "Wrong answer!" << endl;
            }
        } else {
            cout << "No\n";
        }
    }
}