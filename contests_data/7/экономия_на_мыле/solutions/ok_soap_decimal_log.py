from decimal import Decimal, getcontext

getcontext().prec = 200

t = 1

for _ in range(t):
    W, X, A, B = map(Decimal, input().split())

    if X == W:
        print(1)
        continue

    q = (W - X) / W
    log_q = q.ln()
    target = A.ln() - B.ln()

    left, right = 0, 1
    while Decimal(right) * log_q >= target:
        right *= 2

    while left + 1 < right:
        mid = (left + right) // 2
        if Decimal(mid) * log_q < target:
            right = mid
        else:
            left = mid

    k = right

    log_y = W.ln() + B.ln() - A.ln() + Decimal(k) * log_q
    log_x = X.ln()

    m = int((log_y - log_x).exp())
    EPS = Decimal('1e-50')

    while Decimal(m + 1).ln() + log_x <= log_y + EPS:
        m += 1
    while m > 0 and Decimal(m).ln() + log_x > log_y + EPS:
        m -= 1

    print(k + m)