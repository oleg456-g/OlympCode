#include<iostream>
#include<vector>
#include<map>
#include<cmath>

using namespace std;

vector<pair<int, int>> factor(int n, vector<pair<int, int>> b)
{
    if(n == 1) return b;

    vector<pair<int, int>>  c, d;
    int it = 0;
    for(int i = 2; i * i <= n; i++)
    {
        int cnt = 0;
        while(n % i == 0)
        {
            n /= i;
            cnt++;
        }

        while(it < (int)b.size() && b[it].first < i)
        {
            c.push_back(b[it]);

            it++;
        }

        if(it < (int)b.size() && b[it].first == i)
        {
            c.push_back({b[it].first, b[it].second + cnt});
            it++;
        }
        else
        {
            c.push_back({i, cnt});
        }
    }

    if(n != 1)
    {
        while(it < (int)b.size() && b[it].first < n)
        {
            c.push_back(b[it]);

            it++;
        }

        if(it < (int)b.size() && b[it].first == n)
        {
            c.push_back({b[it].first, b[it].second + 1});
            it++;
        }
        else
        {
            c.push_back({n, 1});
        }
    }

    while(it < (int)b.size())
    {
        c.push_back(b[it]);
        it++;
    }

    for(auto &i : c)
    {
        if(i.second % 2)
        {
            d.push_back({i.first, 1});
        }
    }

    return d;
}

int main()
{
    int n;
    cin >> n;
    vector<int> a (n);

    for(auto &i : a)
    {
        cin >> i;
    }

    int ans = 0;
    map<vector<pair<int, int>>, int> was;
    vector<pair<int, int>> current;
    int len = 0;

    for(int i = 0; i < n; i++)
    {
        current = factor(a[i], current);

        if((was.find(current) != was.end() and was[current] < i - 1) or (len > 1 and !current.size()))
        {
            len = 0;
            ans++;
            was.clear();
            current.clear();
        }
        was[current] = i;
        len++;
    }

    ans++;
    cout << ans << endl;
}
