#include<bits/stdc++.h>
using namespace std;

#define int long long

constexpr int MAX_R = 1e5;

signed main(){
    
    int n, q;
    cin>>n>>q;
    map<tuple<int, int, int>, vector<tuple<int, int, int>>> cont;
    vector<int> shift_1_cord = {
        0, 1, -1
    };
    vector<tuple<int, int, int>> shifts;
    for(auto dx : shift_1_cord){
        for(auto dy : shift_1_cord){
            for(auto dz : shift_1_cord){
                shifts.push_back({dx, dy, dz});
            }
        }
    }
    for (int i = 0; i < n; i++)
    {
        int x, y, z;
        cin>>x>>y>>z;
        x--;y--;z--;
        cont[{x/MAX_R, y/MAX_R, z/MAX_R}].push_back({x, y, z});
    }
    
    for (int i = 0; i < q; i++)
    {
        int x, y, z, r;
        cin>>x>>y>>z>>r;
        x--;y--;z--;
        int x1 = x/MAX_R, y1 = y/MAX_R, z1=z/MAX_R;
        auto sqr_dist = [x, y, z](int xx, int yy, int zz){
            return (xx-x)*(xx-x)+(yy-y)*(yy-y)+(zz-z)*(zz-z);
        };
        int ans = 0;
        for(auto [dx, dy, dz] : shifts){
            dx+=x1, dy+=y1, dz+=z1;
            if(!cont.count({dx, dy, dz})) continue;
            for(auto [xx, yy, zz]:cont[{dx, dy, dz}]){

                // cout<<sqr_dist(xx, yy, zz)<<" "<<r*r<<'\n';
                ans += (sqr_dist(xx, yy, zz) < r*r);
            }
        }
        cout<<ans<<'\n';
        
    }
    
    
}