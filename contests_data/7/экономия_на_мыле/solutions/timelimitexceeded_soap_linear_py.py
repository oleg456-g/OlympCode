from math import floor
W, X, A, B = map(int, input().split())

p = A / B
q = (W - X) / W
eps = 1e-12
ans = 0

ratio = 1
s = W

while s >= X:
    s -= X
    ans += 1
    if s == 0:
        break
    # s * ratio / (s + y) = p
    # s * ratio - p * s = p * y
    y = floor(min(s * ratio / p - s + eps, W - s))
    ratio = s * ratio / (s + y)
    s += y

print(ans)
