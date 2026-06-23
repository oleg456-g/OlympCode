#include<bits/stdc++.h>
using namespace std;



constexpr int MAX_R = 1e5;
constexpr int MAX_X = 1e6;
constexpr int LEN_CUBE = 2e4;


signed main(){
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    int n, q;
    cin>>n>>q;
    vector<tuple<int, int, int>> cont[MAX_X/LEN_CUBE][MAX_X/LEN_CUBE][MAX_X/LEN_CUBE];
   
    

   
    for (int i = 0; i < n; i++)
    {
        int x, y, z;
        cin>>x>>y>>z;
        x--;y--;z--;
        cont[x/LEN_CUBE][y/LEN_CUBE][z/LEN_CUBE].push_back({x, y, z});
    }
    
    for (int i = 0; i < q; i++)
    {
        int x, y, z, r;
        cin>>x>>y>>z>>r;
        x--;y--;z--;
        int x1 = x/LEN_CUBE, y1 = y/LEN_CUBE, z1=z/LEN_CUBE;
        auto sqr_dist = [x, y, z](int xx, int yy, int zz) -> long long{
            return 1LL*(xx-x)*(xx-x)+1LL*(yy-y)*(yy-y)+1LL*(zz-z)*(zz-z);
        };
        int ans = 0;
        for(auto dx = max(0, x1-(MAX_R/LEN_CUBE)); dx <= min(x1+MAX_R/LEN_CUBE, MAX_X/LEN_CUBE - 1); dx++){
            for(auto dy = max(0, y1-(MAX_R/LEN_CUBE)); dy <= min(y1+MAX_R/LEN_CUBE, MAX_X/LEN_CUBE - 1); dy++){
                for(auto dz = max(0, z1-(MAX_R/LEN_CUBE)); dz <= min(z1+MAX_R/LEN_CUBE, MAX_X/LEN_CUBE - 1); dz++){

                    for(auto [xx, yy, zz]:cont[dx][dy][dz]){
                        ans += (sqr_dist(xx, yy, zz) < 1LL*r*r);
                    }
                }
            }
        }
        
        cout<<ans<<'\n';
        
    }
    
    
}