#include <iostream>
#include <vector>
#include <assert.h>
 
using namespace std;
 
void swap(int i, int j, vector<int> &p, vector<int> &ind, vector<pair<int, int>> &ans) {
    if (i == j)
        return;
    ans.emplace_back(i + 1, j + 1);
    assert(p[i] == 0);
    int value1 = p[i];
    int value2 = p[j];
    swap(p[i], p[j]);
    ind[value1] = j;
    ind[value2] = i;
}
 
void solve() {
    int n;
    cin >> n;
    vector<int> p(n);
    vector<int> ind(n);
    for (int i = 0; i < n; ++i) {
        cin >> p[i];
        --p[i];
        ind[p[i]] = i;
    }
    if (n == 3) {
        if (ind[2] < ind[1]) {
            cout << -1 << '\n';
            return;
        }
    }
    vector<pair<int, int>> ans;
    for (int i = n - 1; i >= (n + 2) / 2; --i) {
        swap(ind[0], (n) / 2, p, ind, ans);
        swap(ind[0], ind[i], p, ind, ans);
        if (ind[0] == 0)
            swap(ind[0], 1, p, ind, ans);
        swap(ind[0], (n) / 2 + 1, p, ind, ans);
        swap(ind[0], i, p, ind, ans);
        swap(ind[0], ind[i], p, ind, ans);
    }
    swap(ind[0], (n + 2) / 2 - 1, p, ind, ans);
    for (int i = (n + 2) / 2 - 1; i > 0; --i) {
        swap(ind[0], i, p, ind, ans);
        swap(ind[0], ind[i], p, ind, ans);
    }
    cout << ans.size() << '\n';
    for (auto i : ans)
        cout << i.first << ' ' << i.second << '\n';
}
 
int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
 
    int t;
    cin >> t;
    while (t--)
        solve();
}
