#include <stdio.h>

#define MAXM 10000

struct Edge {
    int v, u;
};

int k1, k2;
int n, m;
struct Edge res[MAXM];

void add_edge(int v, int u) {
    res[m].v = v;
    res[m].u = u;
    ++m;
}

void dfs_sochi(int v, int cnt) {
    if (!cnt) return;
    int u1 = n++;
    int u2 = n++;
    add_edge(v, u1);
    add_edge(v, u2);
    add_edge(u1, u2);
    dfs_sochi(u2, --cnt);
}

void solve(void) {
    add_edge(0, 1);
    add_edge(0, 2);
    add_edge(1, 2);
    n = 3;
    for (int i = 0; i < k1; ++i) {
        add_edge(0, n++);
    }
    dfs_sochi(2, --k2);
}

int main() {
    scanf("%d%d", &k1, &k2);
    solve();
    printf("%d %d\n", n, m);
    for (int j = 0; j < m; ++j) {
        printf("%d %d\n", ++res[j].v, ++res[j].u);
    }
}