#include <bits/stdc++.h>
using namespace std;
 
struct Dsu{
    int componentNumber;
    vector<int> parent;
    Dsu() = default;
    Dsu(int n): parent(n), componentNumber(n) {
        iota(parent.begin(), parent.end(), 0);
    
    }
    int root(int v){
            return (parent[v] == v) 
                ? v 
                : parent[v] = root(parent[v]);
    }
    void unite(int a, int b){
        a = root(a), b = root(b);
        if(a == b) return;
        parent[a] = b;
        componentNumber--;
    }
};
 
 
int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
 
    int n, q;
    cin>>n>>q;
    map<int, vector<pair<int, int>>> edges;
    while(q--){
        char c;
        cin>>c;
        if(c == '+'){
            int a, b, k;
            cin>>a>>b>>k;
            a--; b--;
            edges[k].emplace_back(a, b);
        }
        else{
 
            int p;
            cin>>p;
            Dsu d(n);
            if(p == n){
                cout<<0<<'\n';
                continue;
            }
            bool fl = 0;
            for(auto &[k, ms]:edges){
                for(auto [i, j]:ms) d.unite(i, j);
 
                if(d.componentNumber == p){
                    cout<<k<<'\n';
                    fl = 1;
                    break;
                }
            }
            if(!fl){
                cout<<-1<<'\n';
            }
            
        }
    }
}
