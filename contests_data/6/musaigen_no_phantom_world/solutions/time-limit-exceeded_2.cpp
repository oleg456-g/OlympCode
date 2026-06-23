#include <bits/stdc++.h>
using namespace std;

int componentNumber(const vector<vector<int>> &gr){
    vector<int> used(gr.size());
    auto dfs = [&](int v, auto &&dfs) -> void{
        used[v] = 1;
        for(auto u : gr[v]){
            if(used[u]) continue;
            dfs(u, dfs);
        }
    };
    int ans = 0;
    for (int i = 0; i < gr.size(); i++)
    {
        if(!used[i]){
            ans++;
            dfs(i, dfs);
        }
    }
    return ans;
    
}

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
            if(p == n){
                cout<<0<<'\n';
                continue;
            }
            bool fl = 0;
            vector<vector<int>> gr(n);
            for(auto &[k, ms]:edges){
                for(auto [i, j]:ms){
                    gr[i].push_back(j);
                    gr[j].push_back(i);
                }

                if(componentNumber(gr) == p){
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