#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

bool comp(pair <int, int> x, pair <int, int> y) {
    if (y.second < x.second || (x.second == y.second && y.first > x.first)) {
        return true;
    } else {
        return false;
    }
}

int main() {
    int n;
    int k, h;
    cin >> n;
    vector< pair< int, int > > a;
    for (int i = 0; i < n; i++) {
        cin >> k >> h;
        a.push_back(make_pair(k, h));
    }
    sort(a.begin(), a.end(), comp);
    for (int i = 0; i < n; i++) {
        cout << a[i].first << ' ' << a[i].second << '\n';
    }
}