n = int(input())
a = list(map(int, input().split()))
x, y = 0, 0
for i in a:
    if i < 0:
        x -= i
    else:
        y += i
print(max(x, y))
