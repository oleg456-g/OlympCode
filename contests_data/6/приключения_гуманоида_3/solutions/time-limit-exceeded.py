def check():
    ss = len(s)
    for i in range(ss - k + 1):
        x = s[i:i+k].count('6') - s[i:i+k].count('4')
        y = s[i:i+k].count('2') - s[i:i+k].count('8')
        if (x, y) == (n, m):
            return True
    return False

for _ in range(int(input())):
    s = input()
    n, m, k = map(int, input().split())
    if check():
        print('YES')
    else:
        print('NO')
