#include<bits/stdc++.h>
using namespace std;

int main(){
    int k_1, k_2;
    cin>>k_1>>k_2;
    k_2--;
    vector<pair<int, int>> gr;
    int n = 3;
    gr.push_back({1, 2});
    gr.push_back({2, 3});
    gr.push_back({3, 1});
    for (int i = 0; i < k_1; i++)
    {
        int b = ++n;
        gr.push_back({3, b});
    }
    int s = 1, f = 2;
    for (int i = 0; i < k_2; i++)
    {
        int a = ++n; 
        int b = ++n;
        int c = ++n;
        gr.push_back({a, s});
        gr.push_back({a, f});
        gr.push_back({a, b});
        gr.push_back({a, c});
        gr.push_back({b, c});
        s = b;
        f = c;
    }
    cout<<n<<" "<<gr.size()<<"\n";
    for(auto [a, b]:gr){
        cout<<a<<" "<<b<<"\n";
    }
}
