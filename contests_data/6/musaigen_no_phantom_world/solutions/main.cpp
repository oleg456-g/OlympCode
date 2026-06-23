#include <bits/stdc++.h>
using namespace std;


constexpr int SQRT_N = 333;

struct Dsu{
    int componentNumber;
    vector<int> parent, siz;
    Dsu() = default;
    Dsu(int n): parent(n), componentNumber(n), siz(n){
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
        if(siz[a] > siz[b]) swap(a, b);
        siz[b] += siz[a];
        parent[a] = b;
        componentNumber--;
    }
};



struct Block{
    int l, r;
    Dsu ds;
    
};

int main(){
    int n, q;
    cin>>n>>q;
    array<Block, SQRT_N> bl = {};
    auto empt_dsu = Dsu(n);
    for (int i = 0; i < SQRT_N; i++)
    {
        bl[i] = {0, (i+1)*SQRT_N, empt_dsu};
    }
    vector<tuple<int, int, int, int>> query(q);
    vector<tuple<int, int, int>> edgs; 
    for(auto &i : query){
        char c;
        cin>>c;
        if(c == '+'){
            int a, b, k;
            cin>>a>>b>>k;
            a--;b--;
            edgs.push_back({k, a, b});
            i = {0, a, b, k};
        }else{
            int p;
            cin>>p;
            i = {1, p, -1, -1};
        }
    }
    sort(edgs.begin(), edgs.end());
    set<tuple<int, int, int>> edges; 
   
    for(auto &[i, a, b, k]:query){
        if(i == 0){
            int id = distance(edgs.begin(), 
                lower_bound(edgs.begin(), edgs.end(), 
                            make_tuple(k, a, b)));
            for(auto &i : bl){

                if(i.r <= id) continue;
                i.ds.unite(a, b);
            }
            edges.insert({k, a, b});
        }
        else{

            int p = a;
            bool fl = 0;
            for (int i = 0; i < SQRT_N; i++)
            {
                if(bl[i].ds.componentNumber > p) continue;
                if(p == n){
                    cout<<0<<'\n';
                    fl = 1;
                    break;
                }

                auto left = edges.lower_bound(
                    edgs[min(i*SQRT_N, (int)edgs.size()-1)]
                );
                auto right = edges.upper_bound(
                    edgs[min((i+1)*SQRT_N-1, (int)edgs.size()-1)]
                );
                auto bls = (i==0)?Block{0, 0, empt_dsu}:bl[i-1];
                while(left != right){
                    bls.ds.unite(get<1>(*left) , get<2>(*left));
                    if(bls.ds.componentNumber == p){

                        cout<<get<0>(*left)<<'\n';
                        fl = 1;
                        break;
                    }
                    left++;
                }
                break;
            }
            if(!fl) cout<<-1<<'\n';
        }
    }


    
    

    
    
    
}