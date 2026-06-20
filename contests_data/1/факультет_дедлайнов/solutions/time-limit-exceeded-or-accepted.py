import sys
import math

# ---------- sparse table for range maximum ----------
def build_sparse(arr):
    """return (log_table, st) where st[k][i] is max on [i, i+2^k-1]"""
    n = len(arr)
    if n == 0:
        return None, None
    LOG = n.bit_length()
    st = [arr[:]]
    for k in range(1, LOG):
        prev = st[-1]
        step = 1 << (k - 1)
        cur = [max(prev[i], prev[i + step]) for i in range(n - (1 << k) + 1)]
        st.append(cur)
    log = [0] * (n + 1)
    for i in range(2, n + 1):
        log[i] = log[i // 2] + 1
    return log, st

def query_sparse(log, st, l, r):
    """max on inclusive indices l..r, 0‑based, assumes l<=r and non‑empty"""
    k = log[r - l + 1]
    return max(st[k][l], st[k][r - (1 << k) + 1])

# ---------- main solving function ----------
def solve() -> None:
    data = sys.stdin.read().strip().split()
    if not data:
        return
    it = iter(data)
    n = int(next(it))
    m = int(next(it))
    x = int(next(it))
    b = int(next(it))
    c = int(next(it))
    mod = int(next(it))

    # ---- 1. read intervals, build events ----
    events = {}
    for _ in range(n):
        l = int(next(it))
        r = int(next(it))
        events[l] = events.get(l, 0) + 1
        if r + 1 <= m:
            events[r + 1] = events.get(r + 1, 0) - 1
    # ensure borders
    events[1] = events.get(1, 0) + 0
    events[m + 1] = events.get(m + 1, 0) + 0

    sorted_pos = sorted(events.keys())

    segments = []          # (L,R,cnt)
    cur_cnt = 0
    prev = None
    for pos in sorted_pos:
        if prev is not None:
            left = prev
            right = pos - 1
            if left <= right:
                segments.append((left, right, cur_cnt))
        cur_cnt += events[pos]
        prev = pos

    # ---- 2. generate a[i] and find cycle ----
    first_occ = {}
    a_vals = []          # list of values, 0‑based (a[1] is a_vals[0])
    i = 1
    val = x
    while True:
        if val in first_occ:
            start_cycle = first_occ[val]          # 1‑based
            period_len = i - start_cycle
            break
        first_occ[val] = i
        a_vals.append(val)
        i += 1
        val = (val * b + c) % mod

    pre_len = start_cycle - 1
    cycle_len = period_len
    pre_arr = a_vals[:pre_len]
    cycle_arr = a_vals[pre_len:pre_len + cycle_len]

    # ---- 3. build sparse tables ----
    log_pre, st_pre = (None, None)
    if pre_len > 0:
        log_pre, st_pre = build_sparse(pre_arr)
    log_cycle, st_cycle = build_sparse(cycle_arr)
    max_cycle = max(cycle_arr) if cycle_len > 0 else 0

    # ---- helper: max a on interval [L,R] (1‑based) ----
    INF_NEG = -10**30

    def max_a(L, R):
        """maximum a[p] for p in [L,R] (inclusive), 1‑based indices"""
        if L > R:
            return INF_NEG
        ans = INF_NEG
        # part in preperiod
        if L < start_cycle:
            pre_end = min(R, start_cycle - 1)
            if pre_len > 0:
                # convert to 0‑based indices for sparse table
                ans = max(ans, query_sparse(log_pre, st_pre, L - 1, pre_end - 1))
            else:
                # preperiod empty, nothing to query
                pass
            L = start_cycle
        if L > R:
            return ans
        # now L >= start_cycle, only periodic part remains
        length = R - L + 1
        if length >= cycle_len:
            ans = max(ans, max_cycle)
            return ans
        # length < cycle_len
        offL = (L - start_cycle) % cycle_len
        end_off = (offL + length - 1) % cycle_len
        if offL <= end_off:
            ans = max(ans, query_sparse(log_cycle, st_cycle, offL, end_off))
        else:
            ans = max(ans,
                      query_sparse(log_cycle, st_cycle, offL, cycle_len - 1),
                      query_sparse(log_cycle, st_cycle, 0, end_off))
        return ans

    # ---- 4. evaluate all segments ----
    answer = 0
    for L, R, cnt in segments:
        if cnt == 0:
            continue
        best_a = max_a(L, R)
        # best_a can be negative only if no value (should not happen)
        candidate = cnt * best_a
        if candidate > answer:
            answer = candidate

    print(answer)


if __name__ == "__main__":
    solve()
