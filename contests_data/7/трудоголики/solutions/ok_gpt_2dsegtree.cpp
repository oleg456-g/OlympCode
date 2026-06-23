#include <bits/stdc++.h>
using namespace std;

const long long NEG = LLONG_MIN / 4;

struct BIT2D {
    int n;
    vector<vector<long long>> keys;      // sorted keys for each BIT node
    vector<vector<long long>> segDP;     // segment tree for dp values
    vector<vector<long long>> segVal;    // segment tree for transformed values
    vector<int> sz;                     // size (power of two) of segment tree for each node

    BIT2D(int n = 0) : n(n) {
        keys.assign(n + 1, {});
        segDP.assign(n + 1, {});
        segVal.assign(n + 1, {});
        sz.assign(n + 1, 0);
    }

    // add a key that will appear in node idx (called before build)
    void add_key(int idx, long long key) {
        for (int i = idx; i <= n; i += i & -i) keys[i].push_back(key);
    }

    // build segment trees for all nodes
    void build() {
        for (int i = 1; i <= n; ++i) {
            auto &v = keys[i];
            sort(v.begin(), v.end());
            v.erase(unique(v.begin(), v.end()), v.end());
            int m = (int)v.size();
            sz[i] = 1;
            while (sz[i] < m) sz[i] <<= 1;
            segDP[i].assign(2 * sz[i], NEG);
            segVal[i].assign(2 * sz[i], NEG);
        }
    }

    // internal segment tree query on range [l, r] (inclusive)
    static long long querySeg(const vector<long long> &seg, int l, int r, int size) {
        long long ans = NEG;
        int L = l + size;
        int R = r + size;
        while (L <= R) {
            if (L & 1) ans = max(ans, seg[L++]);
            if (!(R & 1)) ans = max(ans, seg[R--]);
            L >>= 1;
            R >>= 1;
        }
        return ans;
    }

    // query maximum dp for keys <= keyMax in prefix [1..idx]
    long long queryDP(int idx, long long keyMax) const {
        long long ans = NEG;
        for (int i = idx; i > 0; i -= i & -i) {
            const auto &v = keys[i];
            int pos = (int)(upper_bound(v.begin(), v.end(), keyMax) - v.begin()) - 1;
            if (pos >= 0) ans = max(ans, querySeg(segDP[i], 0, pos, sz[i]));
        }
        return ans;
    }

    // query maximum stored value for keys in (keyLow, keyHigh] in prefix [1..idx]
    long long queryVal(int idx, long long keyLow, long long keyHigh) const {
        long long ans = NEG;
        for (int i = idx; i > 0; i -= i & -i) {
            const auto &v = keys[i];
            int l = (int)(upper_bound(v.begin(), v.end(), keyLow) - v.begin());          // first > keyLow
            int r = (int)(upper_bound(v.begin(), v.end(), keyHigh) - v.begin()) - 1;    // last <= keyHigh
            if (l <= r) ans = max(ans, querySeg(segVal[i], l, r, sz[i]));
        }
        return ans;
    }

    // point update: set dp = max(old, dpVal), val = max(old, val)
    void update(int idx, long long key, long long dpVal, long long val) {
        for (int i = idx; i <= n; i += i & -i) {
            int pos = (int)(lower_bound(keys[i].begin(), keys[i].end(), key) - keys[i].begin());
            // dp segment tree
            int p = pos + sz[i];
            if (segDP[i][p] < dpVal) {
                segDP[i][p] = dpVal;
                for (p >>= 1; p; p >>= 1) segDP[i][p] = max(segDP[i][p << 1], segDP[i][p << 1 | 1]);
            }
            // val segment tree
            p = pos + sz[i];
            if (segVal[i][p] < val) {
                segVal[i][p] = val;
                for (p >>= 1; p; p >>= 1) segVal[i][p] = max(segVal[i][p << 1], segVal[i][p << 1 | 1]);
            }
        }
    }
};

struct Cow {
    long long a, o, c;
    long long L, R;   // transformed keys
    int pos, revPos;  // compressed positions
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if (!(cin >> n)) return 0;
    vector<Cow> cows(n);
    for (int i = 0; i < n; ++i) {
        cin >> cows[i].a >> cows[i].o >> cows[i].c;
    }
    sort(cows.begin(), cows.end(), [](const Cow &x, const Cow &y) {
        if (x.c != y.c) return x.c < y.c;
        return x.a < y.a;
    });

    // coordinate compression for a
    vector<long long> xs;
    xs.reserve(n);
    for (auto &c : cows) xs.push_back(c.a);
    sort(xs.begin(), xs.end());
    xs.erase(unique(xs.begin(), xs.end()), xs.end());
    int m = (int)xs.size();

    for (auto &c : cows) {
        c.pos = int(lower_bound(xs.begin(), xs.end(), c.a) - xs.begin()) + 1; // 1‑based
        c.revPos = m - c.pos + 1;
        c.L = c.c - c.a;
        c.R = c.c + c.a;
    }

    BIT2D bitLeft(m), bitRight(m);
    for (auto &c : cows) {
        bitLeft.add_key(c.pos, c.L);
        bitRight.add_key(c.revPos, c.R);
    }
    bitLeft.build();
    bitRight.build();

    long long answer = 0;
    for (auto &c : cows) {
        long long a = c.a, o = c.o, cl = c.c;
        long long loL = o - a, hiL = cl - a;
        long long loR = o + a, hiR = cl + a;

        // start from (0,0)
        long long dp = NEG;
        long long startTime = max(a, o);
        long long stay = cl - startTime;
        if (stay > 0) dp = stay;

        // transition: arrive before opening (case 2)
        long long bestDP = bitLeft.queryDP(c.pos, loL);
        if (bestDP != NEG) dp = max(dp, bestDP + (cl - o));

        if (c.revPos > 1) {
            long long bestDP2 = bitRight.queryDP(c.revPos - 1, loR);
            if (bestDP2 != NEG) dp = max(dp, bestDP2 + (cl - o));
        }

        // transition: arrive after opening (case 1)
        if (loL < hiL) {
            long long bestLeft = bitLeft.queryVal(c.pos, loL, hiL);
            if (bestLeft != NEG) dp = max(dp, bestLeft + cl - a);
        }
        if (c.revPos > 1 && loR < hiR) {
            long long bestRight = bitRight.queryVal(c.revPos - 1, loR, hiR);
            if (bestRight != NEG) dp = max(dp, bestRight + cl + a);
        }

        if (dp != NEG) {
            answer = max(answer, dp);
            long long leftVal = dp - cl + a;   // dp - c + a
            long long rightVal = dp - cl - a;  // dp - c - a
            bitLeft.update(c.pos, c.L, dp, leftVal);
            bitRight.update(c.revPos, c.R, dp, rightVal);
        }
    }

    cout << answer << '\n';
    return 0;
}
