#include "testlib.h"
#include <functional>
#include <vector>
#include <set>

using namespace std;

struct Graph{
    int n, m;
    vector<vector<int>> gr;
};

bool checkСonnectivity(Graph &g){
    vector<int> used(g.n); 
    function<int(int)> dfs = [&](int v) {
        used[v] = 1;
        int res = 1;
        for(auto u:g.gr[v]){
            if (used[u]) continue;
            res += dfs(u);
        }
        return res;
    };
    return dfs(0) == g.n;
};

Graph readAns(InStream& stream){
    int n = stream.readInt(3, 10000, "n");
    int m = stream.readInt(3, 10000, "m");
    Graph g = {n, m, vector<vector<int>>(n)}; 
    set<pair<int, int>> edges;
    for (int i = 0; i < m; i++)
    {
        int v = stream.readInt(1, n, "v");
        int u = stream.readInt(1, n, "u");
        if (v == u){
            stream.quitf(_wa, "%d == %d is not allowed", v, u);
        }
        if (edges.count({v, u}) || edges.count({u, v})) {
            stream.quitf(_wa, "edge (%d, %d) was used twice", v, u);
        }
        edges.insert({v, u});
        --v, --u;
        g.gr[v].push_back(u);
        g.gr[u].push_back(v);
    }
    if (!checkСonnectivity(g)){
        stream.quitf(_wa, "graph is not connective");
    }
    return g;
}

int calcBridge(const Graph& g){
    vector<int> d(g.n), h(g.n), used(g.n);
    int timer = 0, countBridge = 0;
    function<void(int,int)> dfs = [&](int v, int p) {
        used[v] = true;
        d[v] = h[v] = timer++;
        
        for (int u : g.gr[v]) {
            if (u != p) {
                if (used[u]) 
                    d[v] = min(d[v], h[u]);
                else { 
                    dfs(u, v);
                    d[v] = min(d[v], d[u]);
                    if (h[v] < d[u]) {
                       countBridge++;
                       
                    }
                }
            }
        }
    };
    dfs(0, -1);
    return countBridge;
}

int calcCutVertex(const Graph& g){
    int timer = 0, countCutVertex = 0;
    vector<int> d(g.n), h(g.n), used(g.n);

    function<void(int, int)> dfs = [&](int v, int p) {
        used[v] = true;
        d[v] = h[v] = timer++;
        int children=0;
        bool ans = 0;
        for (int u : g.gr[v]) {
            if (u != p) {
                if (used[u]) 
                    d[v] = min(d[v], h[u]);
                else { 
                    dfs(u, v);
                    d[v] = min(d[v], d[u]);
                    if (!ans && h[v] <= d[u] && p != -1) {
                        countCutVertex++;
                        ans = 1;
                    }
                    children++;
                }
            }
        }
        if (p == -1 && children > 1){
            countCutVertex++;
        }
    };
    dfs(0, -1);
    return countCutVertex;
}

void readAndCheck(InStream &stream, int k_1, int k_2) {
    Graph graph = readAns(stream);
    int pCutVertex = calcCutVertex(graph);
    int pBridge = calcBridge(graph);
    if (k_1 != pBridge || k_2 != pCutVertex) {
        stream.quitf(_wa, "%d bridges (need %d), %d cut vertexes (need %d)", pBridge, k_1, pCutVertex, k_2);
    }
}

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);
    int k_1 = inf.readInt(); 
    int k_2 = inf.readInt(); 
    readAndCheck(ans, k_1, k_2);
    readAndCheck(ouf, k_1, k_2);
    quitf(_ok, "%d bridges, %d cut vertexes", k_1, k_2);

}

