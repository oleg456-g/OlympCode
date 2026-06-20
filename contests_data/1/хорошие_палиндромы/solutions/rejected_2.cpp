#include <bits/stdc++.h>
using namespace std;

using ull = unsigned long long;

/* ---------- Fenwick tree (BIT) ---------- */
struct Fenwick {
    int n;
    vector<int> bit;
    int maxPow;
    Fenwick(int n = 0) { init(n); }
    void init(int n_) {
        n = n_;
        bit.assign(n + 1, 0);
        maxPow = 1;
        while ((maxPow << 1) <= n) maxPow <<= 1;
    }
    void add(int idx, int delta) {
        for (; idx <= n; idx += idx & -idx) bit[idx] += delta;
    }
    int sum(int idx) const {
        int s = 0;
        for (; idx > 0; idx -= idx & -idx) s += bit[idx];
        return s;
    }
    // 1‑based k, returns smallest idx with prefix sum >= k (k >= 1)
    int find_kth(int k) const {
        int idx = 0;
        for (int pw = maxPow; pw; pw >>= 1) {
            int nxt = idx + pw;
            if (nxt <= n && bit[nxt] < k) {
                k -= bit[nxt];
                idx = nxt;
            }
        }
        return idx + 1;
    }
};

/* ---------- Segment tree for palindrome hash ---------- */
struct Node {
    ull hf, hr;   // forward and reverse hash
    int len;
};

class SegTree {
    int n, size;
    vector<Node> seg;
    vector<ull> powB;
    const ull B = 91138233ULL;   // random odd base
public:
    SegTree(const string& s) {
        n = (int)s.size() - 1;               // s is 1‑based
        size = 1;
        while (size < n) size <<= 1;
        seg.assign(2 * size, {0ULL, 0ULL, 0});
        powB.assign(n + 2, 0ULL);
        powB[0] = 1ULL;
        for (int i = 1; i <= n + 1; ++i) powB[i] = powB[i - 1] * B;
        build(s);
    }
    static Node combine(const Node& L, const Node& R, const vector<ull>& powB) {
        if (L.len == 0) return R;
        if (R.len == 0) return L;
        Node res;
        res.len = L.len + R.len;
        res.hf = L.hf * powB[R.len] + R.hf;
        res.hr = R.hr * powB[L.len] + L.hr;
        return res;
    }
    void build(const string& s) {
        for (int i = 0; i < n; ++i) {
            ull v = (s[i + 1] == 'a') ? 1ULL : 2ULL;
            seg[size + i] = {v, v, 1};
        }
        for (int i = size - 1; i >= 1; --i)
            seg[i] = combine(seg[i << 1], seg[i << 1 | 1], powB);
    }
    void point_update(int pos, char ch) {
        int idx = size + pos - 1;
        ull v = (ch == 'a') ? 1ULL : 2ULL;
        seg[idx] = {v, v, 1};
        for (idx >>= 1; idx >= 1; idx >>= 1)
            seg[idx] = combine(seg[idx << 1], seg[idx << 1 | 1], powB);
    }
    Node range_query(int l, int r) const {
        int L = size + l - 1;
        int R = size + r - 1;
        Node left = {0ULL, 0ULL, 0};
        Node right = {0ULL, 0ULL, 0};
        while (L <= R) {
            if (L & 1) left = combine(left, seg[L++], powB);
            if (!(R & 1)) right = combine(seg[R--], right, powB);
            L >>= 1; R >>= 1;
        }
        return combine(left, right, powB);
    }
};

/* ---------- Main ---------- */
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string t_input;
    if (!(cin >> t_input)) return 0;
    int n = (int)t_input.size();
    string s = " " + t_input;          // 1‑based
    Fenwick bitA(n), bitB(n);
    for (int i = 1; i <= n; ++i) {
        if (s[i] == 'a') bitA.add(i, 1);
        else            bitB.add(i, 1);
    }
    SegTree seg(s);
    int q;  cin >> q;
    while (q--) {
        char type;  cin >> type;
        if (type == '!') {
            int i;  cin >> i;
            char oldc = s[i];
            char newc = (oldc == 'a' ? 'b' : 'a');
            s[i] = newc;
            if (oldc == 'a') {
                bitA.add(i, -1);
                bitB.add(i, +1);
            } else {
                bitA.add(i, +1);
                bitB.add(i, -1);
            }
            seg.point_update(i, newc);
        } else { // '?'
            int l, r;  cin >> l >> r;
            Node nd = seg.range_query(l, r);
            if (nd.hf == nd.hr) {
                cout << 0 << '\n';
                continue;
            }
            int preA_lm1 = bitA.sum(l - 1);
            int preA_r   = bitA.sum(r);
            int cnta = preA_r - preA_lm1;
            int len = r - l + 1;
            int cntb = len - cnta;
            if (cnta == 0 || cntb == 0) {
                cout << 0 << '\n';
                continue;
            }
            int maxD = min( (cnta + 1) / 2 , (cntb + 1) / 2 );
            int preB_lm1 = (l - 1) - preA_lm1;
            int preB_r   = r - preA_r;
            int S = l + r;
            auto exists = [&](int d) -> bool {
                int aL = preA_lm1 + d;
                int aR = preA_r   - d + 1;
                int bL = preB_lm1 + d;
                int bR = preB_r   - d + 1;
                if (aL > aR || bL > bR) return false;
                int A_left  = bitA.find_kth(aL);
                int A_right = bitA.find_kth(aR);
                int B_left  = bitB.find_kth(bL);
                int B_right = bitB.find_kth(bR);
                bool cond1 = max(B_left, S - A_right) <= min(B_right, S - A_left);
                bool cond2 = max(A_left, S - B_right) <= min(A_right, S - B_left);
                return cond1 || cond2;
            };
            int lo = 1, hi = maxD, ans = 0;
            while (lo <= hi) {
                int mid = (lo + hi) >> 1;
                if (exists(mid)) {
                    ans = mid;
                    lo = mid + 1;
                } else hi = mid - 1;
            }
            cout << ans << '\n';
        }
    }
    return 0;
}
