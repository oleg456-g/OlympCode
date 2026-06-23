from decimal import Decimal
from math import floor

t = 1

for _ in range(t):
    w, x, a, b = map(Decimal, input().split())

    if w == x:
        print(1)
        continue

    p = (w - x) / w
    q = a / b

    left_k, right_k = 0, 1

    while p ** right_k >= q:
        right_k *= 2

    while right_k - left_k > 1:
        k = (left_k + right_k) // 2
        if p ** k > q:
            left_k = k
        else:
            right_k = k

    k = left_k
    ans = k + 1
    # print(p, q, k)
    p **= k

    # p * (w - x) = (w - x + y) * a / b
    # p * (w - x) - (w - x) * q = y * q

    y = floor(p * (w - x) / q - (w - x))
    # print(k, y, x)
    assert y <= x
    ans += (w - x + y) // x
    print(ans)



# 889744414 3 1 492
# 560629009 1 1 5911
