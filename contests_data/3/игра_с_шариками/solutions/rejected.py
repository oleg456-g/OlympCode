input()
s = [int(i) for i in input().split()]
sm = 0
for i in s:
    if i > 0:
        sm += i
print(sm)
