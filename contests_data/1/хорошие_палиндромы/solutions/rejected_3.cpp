#include <bits/stdc++.h>
using namespace std;

using ull = unsigned long long;

/*---------------- Fenwick tree ----------------*/
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
    // smallest index with prefix sum >= k (k >= 1)
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

/*---------------- Segment tree for double hash ----------------*/
struct Node {
    ull hf, hr;   // forward and reverse hash, already normalised
    int len;
    Node(ull _hf = 0, ull _hr = 0, int _len = 0) : hf(_hf), hr(_hr), len(_len) {}
};

struct SegTree {
    int n, size;
    vector<Node> seg;
    vector<ull> powB;
    const ull B = 91138233ULL;          // any odd random constant

    SegTree(const string& s) {
        n = (int)s.size() - 1;          // s is 1‑based
        size = 1;
        while (size < n) size <<= 1;
        seg.assign(2 * size, Node());
        powB.assign(n + 2, 0);
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
            seg[size + i] = Node(v, v, 1);
        }
        for (int i = size - 1; i >= 1; --i)
            seg[i] = combine(seg[i << 1], seg[i << 1 | 1], powB);
    }

    void point_update(int pos, char ch) {
        int idx = size + pos - 1;
        ull v = (ch == 'a') ? 1ULL : 2ULL;
        seg[idx] = Node(v, v, 1);
        for (idx >>= 1; idx >= 1; idx >>= 1)
            seg[idx] = combine(seg[idx << 1], seg[idx << 1 | 1], powB);
    }

    // inclusive query [l, r]
    Node range_query(int l, int r) const {
        Node leftRes, rightRes;
        l += size - 1;
        r += size - 1;
        while (l <= r) {
            if (l & 1) leftRes = combine(leftRes, seg[l++], powB);
            if (!(r & 1)) rightRes = combine(seg[r--], rightRes, powB);
            l >>= 1; r >>= 1;
        }
        return combine(leftRes, rightRes, powB);
    }
};

/*---------------- Main ----------------*/
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string t_input;
    if (!(cin >> t_input)) return 0;
    int n = (int)t_input.size();
    string s = " " + t_input;                 // 1‑based

    Fenwick bitA(n), bitB(n);
    for (int i = 1; i <= n; ++i) {
        if (s[i] == 'a') bitA.add(i, 1);
        else            bitB.add(i, 1);
    }

    SegTree seg(s);

    int q;
    cin >> q;
    while (q--) {
        char type;
        cin >> type;
        if (type == '!') {
            int i; cin >> i;
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
            int l, r;
            cin >> l >> r;
            Node whole = seg.range_query(l, r);
            if (whole.hf == whole.hr) {
                cout << 0 << '\n';
                continue;
            }
            int preA_lm1 = bitA.sum(l - 1);
            int preA_r   = bitA.sum(r);
            int cnta = preA_r - preA_lm1;
            int len  = r - l + 1;
            int cntb = len - cnta;
            if (cnta == 0 || cntb == 0) {   // all same character -> palindrome
                cout << 0 << '\n';
                continue;
            }

            int preB_lm1 = (l - 1) - preA_lm1;
            int preB_r   = r - preA_r;

            int hi = max((cnta + 1) / 2, (cntb + 1) / 2); // maximal needed steps
            int lo = 1, ans = hi;
            while (lo <= hi) {
                int mid = (lo + hi) >> 1;   // try k = mid
                // ---- compute intervals after mid steps ----
                int leftA = -1, rightA = -1, leftB = -1, rightB = -1;
                if (cnta > 2 * mid) {
                    int leftRankA  = preA_lm1 + mid + 1;
                    int rightRankA = preA_lm1 + cnta - mid;
                    leftA  = bitA.find_kth(leftRankA);
                    rightA = bitA.find_kth(rightRankA);
                }
                if (cntb > 2 * mid) {
                    int leftRankB  = preB_lm1 + mid + 1;
                    int rightRankB = preB_lm1 + cntb - mid;
                    leftB  = bitB.find_kth(leftRankB);
                    rightB = bitB.find_kth(rightRankB);
                }

                bool ok = false;
                if (leftA == -1 && leftB == -1) {
                    ok = true;                     // empty string
                } else if (leftA == -1) {
                    Node nb = seg.range_query(leftB, rightB);
                    ok = (nb.hf == nb.hr);
                } else if (leftB == -1) {
                    Node na = seg.range_query(leftA, rightA);
                    ok = (na.hf == na.hr);
                } else {
                    int interL = max(leftA, leftB);
                    int interR = min(rightA, rightB);
                    if (interL <= interR) {        // intervals intersect
                        int L = min(leftA, leftB);
                        int R = max(rightA, rightB);
                        Node all = seg.range_query(L, R);
                        ok = (all.hf == all.hr);
                    } else {                       // disjoint
                        // determine which interval is leftmost
                        int leftL, leftR, rightL, rightR;
                        if (leftA < leftB) {
                            leftL = leftA; leftR = rightA;
                            rightL = leftB; rightR = rightB;
                        } else {
                            leftL = leftB; leftR = rightB;
                            rightL = leftA; rightR = rightA;
                        }
                        int lenL = leftR - leftL + 1;
                        int lenR = rightR - rightL + 1;
                        if (lenL == lenR) {
                            Node leftNode  = seg.range_query(leftL, leftR);
                            Node rightNode = seg.range_query(rightL, rightR);
                            ok = (leftNode.hf == rightNode.hr);
                        } else {
                            ok = false;
                        }
                    }
                }

                if (ok) {
                    ans = mid;
                    hi = mid - 1;
                } else {
                    lo = mid + 1;
                }
            }
            cout << ans << '\n';
        }
    }
    return 0;
}
