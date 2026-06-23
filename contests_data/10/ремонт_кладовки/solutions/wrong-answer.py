x, y, z = map(int, input().split())
s1, c1, s2, c2 = map(int, input().split())
a, b = map(int, input().split())

area1 = 2 * (y * z)
area2 = 2 * (x * z)

def calc(need1, need2, s1, c1, s2, c2):
 if c1 > c2:
  s1, c1, s2, c2 = s2, c2, s1, c1
 if s1 >= need1 + need2:
  return (need1 + need2) * c1
 elif s1 >= max(need1, need2) and s2 >= min(need1, need2):
  return min(need1, need2) * c2 + max(need1, need2) * c1
 elif s1 >= min(need1, need2) and s2 >= max(need1, need2):
  return min(need1, need2) * c1 + max(need1, need2) * c2
 else:
  return 10**18

# area1 -= ab

res1, res2 = 10**18, 10**18

# Front wall with area y * z
if a < y and b < z:
 res1 = calc(area1 - a * b, area2, s1, c1, s2, c2)

# Left wall with area x * z
if a < x and b < z:
 res2 = calc(area1, area2 - a * b, s1, c1, s2, c2)
if res1 == 10**18 and res2 == 10**18:
 print(-1)
else:
 print(min(res1, res2))
