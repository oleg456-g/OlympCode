import queue
for _ in range(int(input())):
    n = int(input())
    q = queue.PriorityQueue(maxsize=n * 2)
    for x in input().split():
        q.put(int(x))
    st = {}
    sm = 0
    for i in range(n):
        x = q.get()
        while st.get(x, 0):
            st[x] -= 1
            x = q.get()
        sm += x
        st[sm] = st.get(sm, 0) + 1
        print(x, end=' ')
    print()
