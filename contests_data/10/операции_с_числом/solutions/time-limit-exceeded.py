a, b = map(int, input().split())
A, B = map(int, input().split())
s = input()

n = len(s)
inf = int(1e18)
dp = [[inf, inf] for i in range(n+1)]
dp[0][0] = A;
	

for pref in range(n):
	s10 = 0
	i = pref
	while i >= 0 and (1 << (pref-i)) * a <= inf:
		s10 += (1 << (pref-i)) * (ord(s[i]) - ord('0'))
		if i == 0:
			dp[pref + 1][0] = min(dp[pref+1][0], A + a * s10)
		else:
			dp[pref + 1][0] = min(dp[pref + 1][0], dp[i][1] + (pref-i+1) * b + A + B + s10 * a)
		i = i - 1
	dp[pref + 1][1] = dp[pref + 1][0] + A + B
	if s[pref] == '0':
		dp[pref + 1][1] = min(dp[pref + 1][1], dp[pref][1] + b)

if n == 1 and s[0] == '0':
	print(0)
else:
	print(min(dp[n][0] + A, dp[n][1] + B))
