#include <bits/stdc++.h>
using namespace std;

static const int INF = (int)1e9;
static const int NEG = -(int)1e9;

int id(char c) {
    if (c == 'U') return 0;
    if (c == 'C') return 1;
    if (c == 'A') return 2;
    return 3;
}

int enc(char a, char b, char c) {
    return id(a) * 16 + id(b) * 4 + id(c);
}

int enc(const char *t) {
    return enc(t[0], t[1], t[2]);
}

int enc(const string& s) {
    return enc(s[0], s[1], s[2]);
}

bool is_stop_code(int x) {
    return x == enc("UAA") || x == enc("UAG") || x == enc("UGA");
}

struct Mode {
    int fam;
    int ctype;
    int strat;
};

int n, k, m;
string s;

bool fam_ok[5][64];
bool stop_ok[64];
int AUG_CODE;

vector<int> masks_by_len_pop[6][4];

char pattern_char(int ctype, int idx) {
    if (ctype == 0) return '?';
    if (ctype == 1) {
        static string p = "AUG";
        return p[idx % (int)p.size()];
    }
    if (ctype == 2) {
        static string p = "UAAUGAUAG";
        return p[idx % (int)p.size()];
    }
    if (ctype == 3) {
        static string p = "AAAAUUUUGGGGCCCC";
        return p[idx % (int)p.size()];
    }
    static string p = "AGAGUGUGACAC";
    return p[idx % (int)p.size()];
}

int pos_bias(int strat, int pos, int del_idx) {
    if (strat == 0) return 0;
    if (strat == 1) {
        long long target = 1LL * (del_idx + 1) * (m + 1) / (k + 1);
        return -(int)(llabs((long long)(pos + 1) - target) / 2000);
    }
    if (strat == 2) {
        return -(int)(llabs(2LL * (pos + 1) - (m + 1)) / 2000);
    }
    if (strat == 3) {
        return -(pos + 1) / 3000;
    }
    return -(m - pos) / 3000;
}

void build_families() {
    memset(fam_ok, 0, sizeof(fam_ok));
    memset(stop_ok, 0, sizeof(stop_ok));

    AUG_CODE = enc("AUG");
    stop_ok[enc("UAA")] = true;
    stop_ok[enc("UAG")] = true;
    stop_ok[enc("UGA")] = true;

    for (char a : string("UCAG")) {
        for (char b : string("UCAG")) {
            for (char c : string("UCAG")) {
                int x = enc(a, b, c);
                if (!stop_ok[x] && x != AUG_CODE) fam_ok[0][x] = true;
            }
        }
    }

    vector<string> f1 = {"GCU", "GCC", "GCA", "GCG", "CCU", "CCC", "CCA", "CCG"};
    vector<string> f2 = {"AUG", "UUG", "GUG", "CUG", "UGG", "AGG", "AUA", "AUU"};
    vector<string> f3 = {"AAA", "AAG", "GAA", "GAG", "CAA", "CAG", "UUA", "CUA"};
    vector<string> f4 = {"ACA", "CAA", "AAC", "CAC", "ACC", "CCA"};

    for (auto &t : f1) fam_ok[1][enc(t)] = true;
    for (auto &t : f2) fam_ok[2][enc(t)] = true;
    for (auto &t : f3) fam_ok[3][enc(t)] = true;
    for (auto &t : f4) fam_ok[4][enc(t)] = true;

    for (int len = 3; len <= 5; len++) {
        for (int mask = 0; mask < (1 << len); mask++) {
            int pop = __builtin_popcount((unsigned)mask);
            if (pop <= 2 && len - pop == 3) masks_by_len_pop[len][pop].push_back(mask);
        }
    }
}

struct PreviewRes {
    int matched;
    int score;
    int mode_id;
};

bool code_allowed_preview(int mode_id, int j, int code) {
    static vector<Mode> modes = {
        {0,0,0}, {1,1,1}, {2,2,2}, {3,3,3}, {4,4,4},
        {0,1,0}, {2,1,1}, {3,2,2}, {4,3,3}, {1,4,4}
    };
    const Mode& md = modes[mode_id];
    if (j == 0) return code == AUG_CODE;
    return fam_ok[md.fam][code];
}

bool code_allowed_full(int mode_id, int j, int code) {
    static vector<Mode> modes = {
        {0,0,0}, {1,1,1}, {2,2,2}, {3,3,3}, {4,4,4},
        {0,1,0}, {2,1,1}, {3,2,2}, {4,3,3}, {1,4,4}
    };
    const Mode& md = modes[mode_id];
    if (j == 0) return code == AUG_CODE;
    if (j == n - 1) return stop_ok[code];
    return fam_ok[md.fam][code];
}

inline bool trans_ok(int mode_id, int j, int p, int d, int mask, int len, int &add_score) {
    static vector<Mode> modes = {
        {0,0,0}, {1,1,1}, {2,2,2}, {3,3,3}, {4,4,4},
        {0,1,0}, {2,1,1}, {3,2,2}, {4,3,3}, {1,4,4}
    };
    const Mode& md = modes[mode_id];

    char kept[3];
    int kp = 0;
    int del_used = 0;
    int sc = 1000;
    for (int t = 0; t < len; t++) {
        if (mask >> t & 1) {
            if (md.ctype != 0) {
                if (s[p + t] != pattern_char(md.ctype, d + del_used)) return false;
            }
            sc += pos_bias(md.strat, p + t, d + del_used);
            del_used++;
        } else {
            kept[kp++] = s[p + t];
        }
    }

    int code = enc(kept[0], kept[1], kept[2]);
    if (j >= 0) {
        if (!code_allowed_full(mode_id, j, code)) return false;
    } else {
        if (!code_allowed_preview(mode_id, -j - 1, code)) return false;
    }

    if (del_used == 2) sc -= 7;
    add_score = sc;
    return true;
}

PreviewRes preview_mode(int mode_id, int lim_codons) {
    lim_codons = min(lim_codons, n);
    vector<int> dp(k + 1, NEG), ndp(k + 1, NEG);
    dp[0] = 0;
    int matched = 0;

    for (int j = 0; j < lim_codons; j++) {
        fill(ndp.begin(), ndp.end(), NEG);
        bool any = false;

        for (int d = 0; d <= k; d++) {
            if (dp[d] <= NEG / 2) continue;
            int p = 3 * j + d;
            if (p >= m) continue;

            for (int delta = 0; delta <= 2 && d + delta <= k; delta++) {
                int len = 3 + delta;
                if (p + len > m) continue;
                for (int mask : masks_by_len_pop[len][delta]) {
                    int add = 0;
                    if (!trans_ok(mode_id, -j - 1, p, d, mask, len, add)) continue;
                    ndp[d + delta] = max(ndp[d + delta], dp[d] + add);
                    any = true;
                }
            }
        }

        if (!any) break;
        dp.swap(ndp);
        matched++;
    }

    int best = NEG;
    for (int x : dp) best = max(best, x);
    return {matched, best, mode_id};
}

struct SolveRes {
    bool ok;
    int score;
    vector<int> del;
};

SolveRes solve_mode_full(int mode_id, int max_delta) {
    vector<int> dp(k + 1, NEG), ndp(k + 1, NEG);
    vector<uint16_t> parent((size_t)n * (k + 1), 0xFFFF);

    dp[0] = 0;

    for (int j = 0; j < n; j++) {
        fill(ndp.begin(), ndp.end(), NEG);
        for (int d = 0; d <= k; d++) {
            if (dp[d] <= NEG / 2) continue;
            int p = 3 * j + d;
            if (p >= m) continue;

            for (int delta = 0; delta <= max_delta && d + delta <= k; delta++) {
                int len = 3 + delta;
                if (p + len > m) continue;
                for (int mask : masks_by_len_pop[len][delta]) {
                    int add = 0;
                    if (!trans_ok(mode_id, j, p, d, mask, len, add)) continue;
                    int nd = d + delta;
                    int val = dp[d] + add;
                    if (val > ndp[nd]) {
                        ndp[nd] = val;
                        parent[(size_t)j * (k + 1) + nd] = (uint16_t)(d | (mask << 7));
                    }
                }
            }
        }
        dp.swap(ndp);
    }

    if (dp[k] <= NEG / 2) return {false, NEG, {}};

    vector<int> ans;
    ans.reserve(k);

    int d = k;
    for (int j = n - 1; j >= 0; j--) {
        uint16_t pack = parent[(size_t)j * (k + 1) + d];
        if (pack == 0xFFFF) return {false, NEG, {}};
        int pd = pack & 127;
        int mask = pack >> 7;
        int p = 3 * j + pd;
        int len = 3 + (d - pd);
        for (int t = 0; t < len; t++) {
            if (mask >> t & 1) ans.push_back(p + t + 1);
        }
        d = pd;
    }

    reverse(ans.begin(), ans.end());
    if ((int)ans.size() != k) return {false, NEG, {}};
    return {true, dp[k], ans};
}

int need_one_from_pos(const vector<array<int,4>>& nxt, int p, int a, int b, int c) {
    if (p >= m) return INF;
    int p1 = nxt[p][a];
    if (p1 >= m) return INF;
    int p2 = nxt[p1 + 1][b];
    if (p2 >= m) return INF;
    int p3 = nxt[p2 + 1][c];
    if (p3 >= m) return INF;
    return p3 - p - 2;
}
int need_list_from_pos(const vector<array<int,4>>& nxt, int p, const vector<int>& codes) {
    int res = INF;
    for (int code : codes) {
        int a = code / 16;
        int b = (code / 4) & 3;
        int c = code & 3;
        res = min(res, need_one_from_pos(nxt, p, a, b, c));
    }
    return res;
}

vector<int> fallback_valid() {
    vector<array<int,4>> nxt(m + 2);
    for (int c = 0; c < 4; c++) nxt[m][c] = nxt[m + 1][c] = m;
    for (int i = m - 1; i >= 0; i--) {
        nxt[i] = nxt[i + 1];
        nxt[i][id(s[i])] = i;
    }

    vector<int> mids, stops;
    for (int x = 0; x < 64; x++) {
        if (stop_ok[x]) stops.push_back(x);
        else mids.push_back(x);
    }

    vector<int> needAUG(m + 1, INF), needMid(m + 1, INF), needStop(m + 1, INF);
    for (int p = 0; p <= m; p++) {
        needAUG[p] = need_one_from_pos(nxt, p, id('A'), id('U'), id('G'));
        needMid[p] = need_list_from_pos(nxt, p, mids);
        needStop[p] = need_list_from_pos(nxt, p, stops);
    }

    vector<int> L(n + 1, INF);
    L[0] = 0;
    if (needAUG[0] > k) {
        vector<int> ans;
        for (int i = 1; i <= k; i++) ans.push_back(i);
        return ans;
    }
    L[1] = needAUG[0];

    for (int j = 1; j <= n - 2; j++) {
        int best = INF;
        int base = 3 * j;
        for (int d = L[j]; d <= k; d++) {
            int p = base + d;
            if (p <= m) best = min(best, d + needMid[p]);
        }
        L[j + 1] = best;
        if (L[j + 1] > k) break;
    }

    if (L[n - 1] <= k) {
        int best = INF;
        int base = 3 * (n - 1);
        for (int d = L[n - 1]; d <= k; d++) {
            int p = base + d;
            if (p <= m) best = min(best, d + needStop[p]);
        }
        L[n] = best;
    }

    if (L[n] > k) {
        vector<int> ans;
        for (int i = 1; i <= k; i++) ans.push_back(i);
        return ans;
    }

    vector<int> D(n + 1, 0);
    D[n] = k;

    for (int j = n - 1; j >= 1; j--) {
        bool ok = false;
        int low = L[j], high = D[j + 1];
        for (int d = high; d >= low; d--) {
            int p = 3 * j + d;
            int need = (j == n - 1 ? needStop[p] : needMid[p]);
            if (need <= D[j + 1] - d) {
                D[j] = d;
                ok = true;
                break;
            }
        }
        if (!ok) {
            vector<int> ans;
            for (int i = 1; i <= k; i++) ans.push_back(i);
            return ans;
        }
    }

    auto find_keep = [&](int p, int delta, const vector<int>& codes) -> array<int,3> {
        int lim = p + delta + 2;
        array<int,3> best = {-1, -1, -1};
        int bestEnd = INF;
        for (int code : codes) {
            int a = code / 16;
            int b = (code / 4) & 3;
            int c = code & 3;
            int p1 = nxt[p][a];
            if (p1 >= m || p1 > lim) continue;
            int p2 = nxt[p1 + 1][b];
            if (p2 >= m || p2 > lim) continue;
            int p3 = nxt[p2 + 1][c];
            if (p3 >= m || p3 > lim) continue;
            if (p3 < bestEnd) {
                bestEnd = p3;
                best = {p1, p2, p3};
            }
        }
        return best;
    };

    vector<int> augs = {AUG_CODE};
    vector<int> ans;
    ans.reserve(k);

    for (int j = 0; j < n; j++) {
        int d0 = D[j], d1 = D[j + 1];
        int p = 3 * j + d0;
        int delta = d1 - d0;
        int r = p + 3 + delta - 1;

        array<int,3> keep;
        if (j == 0) keep = find_keep(p, delta, augs);
        else if (j == n - 1) keep = find_keep(p, delta, stops);
        else keep = find_keep(p, delta, mids);

        if (keep[0] == -1) {
            vector<int> tmp;
            for (int i = 1; i <= k; i++) tmp.push_back(i);
            return tmp;
        }

        for (int x = p; x <= r; x++) {
            if (x != keep[0] && x != keep[1] && x != keep[2]) ans.push_back(x + 1);
        }
    }

    if ((int)ans.size() != k) {
        vector<int> tmp;
        for (int i = 1; i <= k; i++) tmp.push_back(i);
        return tmp;
    }
    return ans;
}
void solve() {

    cin >> n >> k;
    cin >> s;
    m = (int)s.size();

    vector<int> mode_ids;
    if (n > 5000) {
        mode_ids = {5, 6, 7, 8, 9};
    } else {
        mode_ids = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    }

    vector<PreviewRes> prevs;
    int lim = min(n, 2000);
    for (int mid : mode_ids) prevs.push_back(preview_mode(mid, lim));

    sort(prevs.begin(), prevs.end(), [](const PreviewRes& A, const PreviewRes& B) {
        if (A.matched != B.matched) return A.matched > B.matched;
        return A.score > B.score;
    });

    vector<int> cand;
    for (int i = 0; i < (int)prevs.size() && i < 3; i++) cand.push_back(prevs[i].mode_id);

    SolveRes best = {false, NEG, {}};

    for (int mid : cand) {
        SolveRes cur = solve_mode_full(mid, 2);
        if (cur.ok && cur.score > best.score) best = cur;
    }

    if (!best.ok) {
        for (int mid : cand) {
            SolveRes cur = solve_mode_full(mid, 3);
            if (cur.ok && cur.score > best.score) best = cur;
        }
    }

    vector<int> ans;
    if (best.ok) ans = best.del;
    else ans = fallback_valid();

    for (int i = 0; i < k; i++) {
        if (i) cout << ' ';
        cout << ans[i];
    }
    cout << '\n';
    return;
}

int main() {
    build_families();
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
}