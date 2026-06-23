def f(p1, p2, p3):
    an = float('inf')
    if s1 >= p1 + p2 + p3 + p3:
        an = min(an, c1 * (p1 + p2 + p3 + p3))
    if s2 >= p1 + p2 + p3 + p3:
        an = min(an, c2 * (p1 + p2 + p3 + p3))
    if s1 >= p1 + p2 and s2 >= p3 + p3:
        an = min(an, c1 * (p1 + p2) + c2 * (p3 + p3))
    if s2 >= p1 + p2 and s1 >= p3 + p3:
        an = min(an, c2 * (p1 + p2) + c1 * (p3 + p3))
    return an


x, y, z = map(int, input().split())
s1, c1, s2, c2 = map(int, input().split())
a, b = map(int, input().split())

ans = float('inf')
if a <= x and b <= z:
    ans = min(ans, f(x * z - a * b, x * z, y * z))
if a <= y and b <= z:
    ans = min(ans, f(y * z - a * b, y * z, x * z))

if ans == float('inf'):
    print(-1)
else:
    print(ans)
