
import sys

MOD = 10**9 + 7
EDGES = 8


def main() -> None:
    data = sys.stdin.buffer.read().split()
    if not data:
        return
    it = iter(data)
    n = int(next(it))
    L = int(next(it))

    raw = [next(it).decode() for _ in range(n)]
    letters = sorted({ch for s in raw for ch in s})
    mp = {ch: i for i, ch in enumerate(letters)}
    E = len(letters)  # именно ALPH, чтобы было O(ALPH^5)

    cnt = [[0] * E for _ in range(E)]
    palindrome = [0] * E

    for s in raw:
        rs = s[::-1]
        if rs < s:
            s, rs = rs, s
        if s == rs:
            palindrome[mp[s[0]]] += 1
        else:
            cnt[mp[s[0]]][mp[s[L - 1]]] += 1

    pw = [1] * (EDGES + 1)
    for i in range(1, EDGES + 1):
        pw[i] = 2 * pw[i - 1]

    pascal = [[0] * (EDGES + 1) for _ in range(EDGES + 1)]
    for i in range(EDGES + 1):
        pascal[i][0] = 1
        pascal[i][i] = 1
        for j in range(1, i):
            pascal[i][j] = pascal[i - 1][j] + pascal[i - 1][j - 1]

    placements = [[[0] * (EDGES + 1) for _ in range(E)] for __ in range(E)]
    for a in range(E):
        for b in range(a, E):
            placements[a][b][0] = 1
            for i in range(1, EDGES + 1):
                placements[a][b][i] = placements[a][b][i - 1] * (cnt[a][b] - i + 1) % MOD

    place_pal = [[0] * (EDGES + 1) for _ in range(E)]
    for a in range(E):
        place_pal[a][0] = 1
        for i in range(1, EDGES + 1):
            place_pal[a][i] = place_pal[a][i - 1] * (palindrome[a] - i + 1) % MOD

    cur = [[0] * E for _ in range(E)]
    used = [[0] * E for _ in range(E)]
    uniques = []

    def add(x: int, y: int) -> None:
        if x > y:
            x, y = y, x
        cur[x][y] += 1
        if not used[x][y]:
            uniques.append((x, y))
            used[x][y] = 1

    def rm(x: int, y: int) -> None:
        if x > y:
            x, y = y, x
        cur[x][y] = 0
        used[x][y] = 0

    ans = 0
    for u in range(E):
        for a in range(E):
            for b in range(E):
                for c in range(E):
                    for d in range(E):
                        edges = ((u, a), (u, b), (u, c), (u, d), (a, b), (b, c), (c, d), (d, a))
                        for x, y in edges:
                            add(x, y)

                        num = 1
                        for x, y in uniques:
                            if x == y:
                                ways = 0
                                need = cur[x][x]
                                lim = min(need, palindrome[x])
                                for i in range(lim + 1):
                                    ways = (ways + place_pal[x][i] * placements[x][x][need - i] % MOD * pw[need - i] * pascal[need][i]) % MOD
                                num = (num * ways) % MOD
                            else:
                                num = (num * placements[x][y][cur[x][y]]) % MOD
                            if not num:
                                break
                        ans = (ans + num) % MOD

                        for x, y in uniques:
                            rm(x, y)
                        uniques.clear()

    print(ans)


if __name__ == "__main__":
    main()