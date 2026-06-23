import sys
import math
from decimal import Decimal, getcontext, ROUND_FLOOR

# -------------------------------------------------------------

def solve() -> None:
    W, X, A, B = map(int, sys.stdin.readline().split())

    # X == W : after the first press the dispenser is empty,
    #          no water can be added, the answer is 1.
    if X == W:
        print(1)
        return

    # fraction must stay 1 -> no water ever can be added
    if A == B:
        print(W // X)
        return

    # ----- Phase 1 : find maximal t with B * r^t >= A -----
    r_float = (W - X) / W                # 0 < r < 1
    log_r = math.log(r_float)            # negative
    log_B = math.log(B)
    log_A = math.log(A)

    # floating test, small epsilon to compensate rounding
    eps = 1e-12
    def cond_float(t: int) -> bool:
        return log_B + t * log_r >= log_A - eps

    low = 0
    high = 1
    while cond_float(high):
        high <<= 1                       # double until condition fails

    while low < high:
        mid = (low + high + 1) // 2
        if cond_float(mid):
            low = mid
        else:
            high = mid - 1
    t = low                               # candidate for maximal t

    # ----- correct t using exact arithmetic (Decimal) -----
    getcontext().prec = 200                # enough for all calculations
    D_W = Decimal(W)
    D_B = Decimal(B)
    D_A = Decimal(A)
    D_r = Decimal(W - X) / D_W

    # B * r^t  (Decimal)
    D_r_pow_t = D_r ** t
    left = D_B * D_r_pow_t

    # adjust downwards if needed
    while left < D_A:
        t -= 1
        D_r_pow_t = D_r ** t
        left = D_B * D_r_pow_t

    # adjust upwards if possible (very unlikely, but safe)
    while True:
        nxt = D_r ** (t + 1)
        if D_B * nxt >= D_A:
            t += 1
            D_r_pow_t = nxt
        else:
            break

    # ----- compute L1 = floor( (B * W * r^{t+1}) / A ) -----
    D_r_pow_tp1 = D_r ** (t + 1)
    D_val = (D_B * D_W * D_r_pow_tp1) / D_A
    L1 = int(D_val.to_integral_value(rounding=ROUND_FLOOR))

    # ----- final answer -----
    answer = t + 1 + (L1 // X)
    print(answer)


# -------------------------------------------------------------
if __name__ == "__main__":
    solve()
