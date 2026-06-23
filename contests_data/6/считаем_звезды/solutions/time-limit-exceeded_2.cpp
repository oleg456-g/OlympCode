#include<bits/stdc++.h>
using namespace std;

#define int long long

constexpr int MAX_R = 1e5;

signed main(){
    
    int n, q;
    cin>>n>>q;
    map<int, vector<tuple<int, int, int>>> cont;

    for (int i = 0; i < n; i++)
    {
        int x, y, z;
        cin>>x>>y>>z;
        x--;y--;z--;
        cont[x].push_back({x, y, z});
    }
    
    for (int i = 0; i < q; i++)
    {
        int x, y, z, rad;
        cin>>x>>y>>z>>rad;
        x--;y--;z--;
        int x1 = x/MAX_R, y1 = y/MAX_R, z1=z/MAX_R;
        auto sqr_dist = [x, y, z](int xx, int yy, int zz){
            return (xx-x)*(xx-x)+(yy-y)*(yy-y)+(zz-z)*(zz-z);
        };
        int ans = 0;
        auto l = cont.lower_bound(x-MAX_R);
        auto r = cont.lower_bound(x+MAX_R);

        for(auto i = l; i != r; i++){
            auto &[sm, mas] = *i;
            for(auto [xx, yy, zz] : mas){
                ans += (sqr_dist(xx, yy, zz) < rad*rad);
            }
        }

        cout<<ans<<'\n';
    }
    
    
}