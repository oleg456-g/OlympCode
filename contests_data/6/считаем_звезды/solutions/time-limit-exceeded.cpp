#include<bits/stdc++.h>
using namespace std;

#define int long long

constexpr int MAX_R = 1e5;

signed main(){
    
    int n, q;
    cin>>n>>q;
    vector<tuple<int, int, int>> cont;
    vector<int> shift_1_cord = {
        0, 1, -1
    };
    
    for (int i = 0; i < n; i++)
    {
        int x, y, z;
        cin>>x>>y>>z;
        x--;y--;z--;
        cont.push_back({x, y, z});
    }
    
    for (int i = 0; i < q; i++)
    {
        int x, y, z, r;
        cin>>x>>y>>z>>r;
        x--;y--;z--;
        
        auto sqr_dist = [x, y, z](int xx, int yy, int zz){
            return (xx-x)*(xx-x)+(yy-y)*(yy-y)+(zz-z)*(zz-z);
        };
        int ans = 0;
        
        for(auto [xx, yy, zz] : cont){
            ans += (sqr_dist(xx, yy, zz) < r*r);
        }
        
        cout<<ans<<'\n';
        
    }
    
    
}