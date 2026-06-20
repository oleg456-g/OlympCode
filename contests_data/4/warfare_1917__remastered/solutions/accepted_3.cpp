#include <algorithm>
#include <bits/stdc++.h>
#include <numeric>
#include <queue>
#include <random>

using namespace std;
using ll = long long;
using pi = pair<int, int>;
using vi = vector<int>;

template<typename T>
bool ckmin(T& a, const T& b) { return b < a ? a = b, true : false; }
template<typename T>
bool ckmax(T& a, const T& b) { return a < b ? a = b, true : false; }

template<typename T>
using pq_min = priority_queue<T, vector<T>, greater<T>>;
template<typename T>
using pq_max = priority_queue<T, vector<T>, less<T>>;

constexpr int N = 6;
constexpr int B = 8;
constexpr int F = 6*5*4*3*2*1;
map<vi, int> perm_pos;
vi perm_ord[F];
int game[F][F];
double bot_prob[B][F];

enum {
    BOT_SORTED,
    BOT_REVERS,
    BOT_HALF_AB,
    BOT_HALF_BA,
    BOT_2FOR,
    BOT_2REV,
    BOT_3FOR,
    BOT_3REV,
};

void precalc_bots() {
    vi p(N);
    iota(p.begin(), p.end(), 1);
    int pos = 0;
    do {
        perm_ord[pos] = p;
        perm_pos[p] = pos;
        pos++;
    } while (next_permutation(p.begin(), p.end()));
    assert(pos == F);

    bot_prob[BOT_SORTED][perm_pos[vi{1,2,3,4,5,6}]] = 1;
    bot_prob[BOT_REVERS][perm_pos[vi{6,5,4,3,2,1}]] = 1;

    vi a{{1,2,3}}, b{{4,5,6}};
    do {
        do {
            vi p, q;
            for (int x : a) p.push_back(x); for (int x : b) p.push_back(x);
            bot_prob[BOT_HALF_AB][perm_pos[p]] = double(1)/36;

            for (int x : b) q.push_back(x); for (int x : a) q.push_back(x);
            bot_prob[BOT_HALF_BA][perm_pos[q]] = double(1)/36;

        } while(next_permutation(b.begin(), b.end()));
    } while(next_permutation(a.begin(), a.end()));

    for (int msk = 1 << 5; msk--; ) {
        pq_min<int> q;
        for (int i = 1; i <= N; ++i)
            q.push(i);

        vector<int> p;
        for (int i = 0; i < 5; ++i) {
            int x = q.top(); q.pop();
            int y = q.top(); q.pop();
            if ((msk >> i) & 1) swap(x, y);
            q.push(x);
            p.push_back(y);
        }
        p.push_back(q.top());

        auto t = p;
        for (int& x : t) x = N + 1 - x;

        bot_prob[BOT_2FOR][perm_pos[p]] = double(1)/32;
        bot_prob[BOT_2REV][perm_pos[t]] = double(1)/32;
    }

    for (int i1 = 0; i1 < 3; ++i1) {
        for (int i2 = 0; i2 < 3; ++i2) {
            for (int i3 = 0; i3 < 3; ++i3) {
                for (int i4 = 0; i4 < 3; ++i4) {
                    for (int j5 = 0; j5 < 2; ++j5) {
                        pq_min<int> q;
                        for (int i = 1; i <= N; ++i) q.push(i);

                        vector<int> p;
                        array<int, 4> moves{{i1,i2,i3,i4}};
                        for (int m = 0; m < 4; ++m) {
                            array<int, 3> mns;
                            mns[0] = q.top(); q.pop();
                            mns[1] = q.top(); q.pop();
                            mns[2] = q.top(); q.pop();
                            swap(mns[moves[m]], mns[0]);
                            q.push(mns[1]);
                            q.push(mns[2]);
                            p.push_back(mns[0]);
                        }

                        {
                            int x = q.top(); q.pop();
                            int y = q.top(); q.pop();
                            if (j5) swap(x, y);
                            q.push(x);
                            p.push_back(y);
                        }

                        p.push_back(q.top());
                        auto t = p;
                        for (int& x : t) x = N + 1 - x;

                        bot_prob[BOT_3FOR][perm_pos[p]] = double(1)/162;
                        bot_prob[BOT_3REV][perm_pos[t]] = double(1)/162;
                    }
                }
            }
        }
    }
}

void precalc_game() {
    for (int i = 0; i < F; ++i) {
        for (int j = 0; j < F; ++j) {
            auto p = perm_ord[i], q = perm_ord[j];

            for (int t = 0; t < N; ++t) {
                game[i][j] += p[t] > q[t] ? 1 : 0;
                game[i][j] -= p[t] < q[t] ? 1 : 0;
            }
        }
    }
}

int wins = 0;
namespace local_bot {

int bot_id;
mt19937 rng(1337228);
bool verbose = false;

int move() {
    double x = uniform_real_distribution<double>()(rng);
    for (int i = 0; i < F; ++i) {
        if (x < bot_prob[bot_id][i]) {
            return i;
        } else {
            x -= bot_prob[bot_id][i];
        }
    }
    return 0;
}

int ask(int player) {
    int bot = local_bot::move();

    if (verbose) {
        cerr << "bot=";
        for (int x : perm_ord[bot])
            cerr << x;
        cerr << ", player=";
        for (int x : perm_ord[player])
            cerr << x;
        cerr << " => " << game[player][bot] << endl;
    }

    if (game[player][bot] > 0) ++wins;

    return bot;
}

} // local_bot

namespace interact {

int ask(int player) {
    vector<int> p = perm_ord[player];
    vector<int> q(N);
    for (int i = 0; i < N; ++i) {
        cout << p[i] << endl;
        cin >> q[i];
    }
    int bot = perm_pos[q];
    if (game[player][bot] > 0) ++wins;
    return bot;
}

}

int ask(int player) {
    return interact::ask(player);
}

void solve() {
    precalc_bots();
    precalc_game();
    // cin >> local_bot::bot_id;


    array<double, B> prob;
    for (int i = 0; i < B; ++i) prob[i] = double(1)/B;

    constexpr int GAMES = 500;

    bool guessed = false;
    int player_move = 0;
    for (int i = 0; i < GAMES; ++i) {
        double best_p = 0;
        if (!guessed) {
            int cnt_bots = 0;
            for (int player = 0; player < F; ++player) {
                double win_p = 0;
                for (int bot = 0; bot < B; ++bot) {
                    if (prob[bot] < 1e-4) continue;

                    ++cnt_bots;
                    double win_bot = 0;
                    for (int bm = 0; bm < F; ++bm)
                        win_bot += game[player][bm] > 0 ? bot_prob[bot][bm] : 0;
                    win_p += win_bot * prob[bot];
                }
                if (win_p > best_p) {
                    best_p = win_p;
                    player_move = player;
                }
            }
            cnt_bots /= F;
            if (cnt_bots == 1) guessed = true;
        }

        int bot = ask(player_move);
        for (int i = 0; i < B; ++i)
            prob[i] *= bot_prob[i][bot];
        double s = accumulate(prob.begin(), prob.end(), 0.0);
        for (auto& x : prob) x /= s;

        // for (double x : prob) cout << x << ' ';
        // cout << endl;
    }
    cerr << "total wins: " << wins << endl;
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0);
    int t = 1;
    // cin >> t;
    while (t--) solve();
}