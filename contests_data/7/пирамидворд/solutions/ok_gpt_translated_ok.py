#!/usr/bin/env python3
import sys

MOD = 1_000_000_007


def falling(x: int, k: int) -> int:
    """x·(x‑1)…(x‑k+1)   (mod MOD)   – 0 if k > x."""
    if k > x:
        return 0
    res = 1
    for i in range(k):
        res = (res * ((x - i) % MOD)) % MOD
    return res


def generate_partitions():
    """
    All partitions of the set {0,…,4}.
    Each partition is represented by a list `cls` of length 5,
    where `cls[v]` is the index of the block that contains vertex v.
    """
    res = []
    blocks = []

    def rec(v: int):
        if v == 5:
            cls = [0] * 5
            for i, block in enumerate(blocks):
                for x in block:
                    cls[x] = i
            res.append(cls)
            return
        # put v into an existing block
        for block in blocks:
            block.append(v)
            rec(v + 1)
            block.pop()
        # start a new block with v
        blocks.append([v])
        rec(v + 1)
        blocks.pop()

    rec(0)
    return res


def main() -> None:
    data = sys.stdin.buffer.read().split()
    if not data:
        return
    it = iter(data)
    n = int(next(it))
    _ = int(next(it))                     # L – not needed for the solution

    # ----- read strings -------------------------------------------------
    cnt_diff = [[0] * 26 for _ in range(26)]   # a != b, a<b
    pal = [0] * 26
    nonpal = [0] * 26

    for _ in range(n):
        s = next(it).decode()
        a = ord(s[0]) - 97
        b = ord(s[-1]) - 97
        if a == b:
            if s == s[::-1]:
                pal[a] += 1
            else:
                nonpal[a] += 1
        else:
            if a > b:
                a, b = b, a
            cnt_diff[a][b] += 1

    # ----- binomial coefficients C[n][k] for n,k ≤ 8 --------------------
    C = [[0] * 9 for _ in range(9)]
    for i in range(9):
        C[i][0] = C[i][i] = 1
        for j in range(1, i):
            C[i][j] = (C[i - 1][j - 1] + C[i - 1][j]) % MOD

    # ----- diff[i][j][k] – ways to choose k distinct strings
    #        whose unordered ends are i and j (i may equal j) -------------
    diff = [[[0] * 9 for _ in range(26)] for _ in range(26)]
    for i in range(26):
        for j in range(26):
            diff[i][j][0] = 1                     # empty choice
    for i in range(26):
        for j in range(i + 1, 26):
            cnt = cnt_diff[i][j]
            for k in range(1, 9):
                val = falling(cnt, k)
                diff[i][j][k] = diff[j][i][k] = val

    # ----- same[x][k] – ways to assign k loop‑edges to a vertex of colour x
    pow2 = [1] * 9
    for i in range(1, 9):
        pow2[i] = (pow2[i - 1] * 2) % MOD

    same = [[0] * 9 for _ in range(26)]
    for x in range(26):
        same[x][0] = 1
        for k in range(1, 9):
            total = 0
            for r in range(k + 1):                 # r non‑palindromic strings
                ways = C[k][r] * falling(nonpal[x], r) % MOD
                ways = ways * pow2[r] % MOD
                ways = ways * falling(pal[x], k - r) % MOD
                total = (total + ways) % MOD
            same[x][k] = total

    # letters that have a non‑zero `same` for a given loop count
    letters_by_loop = [[] for _ in range(9)]
    for k in range(9):
        for x in range(26):
            if same[x][k]:
                letters_by_loop[k].append(x)

    # ----- the fixed graph (square + centre) ----------------------------
    edges = [(0, 1), (1, 2), (2, 3), (3, 0),
             (0, 4), (1, 4), (2, 4), (3, 4)]

    # ----- all partitions of the 5 vertices -----------------------------
    raw_parts = generate_partitions()
    parts = []
    for cls in raw_parts:
        b = max(cls) + 1                      # number of blocks
        loops = [0] * b
        deg = [0] * b
        mult = [[0] * b for _ in range(b)]

        for u, v in edges:
            a = cls[u]
            c = cls[v]
            if a == c:                        # edge becomes a loop
                loops[a] += 1
                deg[a] += 2
            else:                             # edge between two blocks
                mult[a][c] += 1
                mult[c][a] += 1
                deg[a] += 1
                deg[c] += 1

        # order blocks – heavy blocks first (helps pruning)
        order = list(range(b))
        order.sort(key=lambda i: (-deg[i], -loops[i]))

        loops_arr = [loops[order[i]] for i in range(b)]
        prev = [[] for _ in range(b)]          # edges to already processed blocks
        for i in range(b):
            for j in range(i):
                m = mult[order[j]][order[i]]
                if m:
                    prev[i].append((j, m))

        parts.append((b, loops_arr, prev))

    # ----- depth‑first search over colourings ---------------------------
    ans = 0
    used = [0] * 26
    colour = [0] * 5
    sys.setrecursionlimit(10000)

    def dfs(part, pos, cur):
        nonlocal ans
        b, loops_arr, prev = part
        if pos == b:                     # all blocks coloured
            ans += cur
            if ans >= MOD:
                ans -= MOD
            return

        need = loops_arr[pos]
        for x in letters_by_loop[need]:
            if used[x]:
                continue
            ways = same[x][need]
            ok = True
            for j, m in prev[pos]:
                add = diff[x][colour[j]][m]
                if add == 0:
                    ok = False
                    break
                ways = ways * add % MOD
            if not ok:
                continue
            used[x] = 1
            colour[pos] = x
            dfs(part, pos + 1, cur * ways % MOD)
            used[x] = 0

    for part in parts:
        dfs(part, 0, 1)

    print(ans % MOD)


if __name__ == '__main__':
    main()
