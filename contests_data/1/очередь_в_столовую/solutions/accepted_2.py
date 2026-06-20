import sys

def solve() -> None:
    data = sys.stdin.buffer.read().split()
    it = iter(data)
    t = int(next(it))
    out = []
    for _ in range(t):
        n = int(next(it))
        d = int(next(it))
        cum = 0               # sum of k_i of people already processed
        last_bad = 0          # last (1‑based) position with slack < d
        for i in range(1, n + 1):
            ti = int(next(it))
            ki = int(next(it))
            slack = ti - cum   # t_i - waiting_time_before_i
            if slack < d:
                last_bad = i
            cum += ki
        out.append(str(last_bad + 1))   # minimal possible position
    sys.stdout.write("\n".join(out))

if __name__ == "__main__":
    solve()
