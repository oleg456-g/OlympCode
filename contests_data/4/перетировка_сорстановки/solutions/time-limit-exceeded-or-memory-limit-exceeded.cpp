#include <iostream>
#include <iterator>
#include <utility>
#include <vector>
#include <algorithm>
#include <map>
#include <deque>
using namespace std;


void swap_min_max(vector<int> &arr, int i, int j) {
    int idx1 = i, idx2 = i;
    for (int a = i; a <= j; a++) {
        if (arr[a] < arr[idx1]) {
            idx1 = a;
        }
        if (arr[a] > arr[idx2]) {
            idx2 = a;
        }
    }
    swap(arr[idx1], arr[idx2]);
}

int main() {
    int t;
    cin >> t;
    while (t--) {
        int n, k;
        cin >> n >> k;
        vector<int> arr(n);
        for (int i = 0; i < n; i++) {
            cin >> arr[i];
            arr[i]--;
        }
        map<vector<int>, int> pr;
        pr[arr] = -1;
        deque<vector<int>> dq;
        dq.push_back(arr);
        while (!dq.empty()) {
            vector<int> v = dq.front();
            dq.pop_front();
            for (int i = 0; i + k <= n; i++) {
                vector<int> to = v;
                swap_min_max(to, i, i + k - 1);
                if (pr.find(to) == pr.end()) {
                    pr[to] = i;
                    dq.push_back(to);
                }
            }
        }
        vector<int> id(n);
        for (int i = 0; i < n; i++) id[i]  = i ;
        if (pr.find(id) == pr.end()) {
            cout << "NO\n";
            continue;
        }
        cout << "YES\n";
        vector<int> v = id;
        vector<pair<int, int>> ans;
        while (1) {
            if (pr[v] == -1) {
                break;
            }
            int i = pr[v];
            int j = pr[v] + k - 1;
            ans.push_back({i, j});
            swap_min_max(v, i, j);
        }
        reverse(ans.begin(), ans.end());
        cout << ans.size() << '\n';
        for (auto el: ans) {
            cout << el.first + 1 << ' ' << el.second + 1 << '\n';
        }
    }
}