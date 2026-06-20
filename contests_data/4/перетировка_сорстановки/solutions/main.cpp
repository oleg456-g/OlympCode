// #define _GLIBCXX_DEBUG
#pragma GCC optimize("O3")
// #include "testlib.h"
#include <deque>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
#include <set>
#include <map>
#include <algorithm>

using namespace std;

map<vector<int>, int> pr[8][8];

vector<pair<int, int>> sort(int n, int k, vector<int> arr) {
    vector<pair<int, int>> ans;
    while (1) {
        // for (auto el: arr) {
        //     cout << el << ' ';
        // }
        // cout << '\n';
        // cout << '@' << n << ' ' << k << ' ' << pr[n][k][arr] << '\n';
        if (pr[n][k][arr] == -1) {
            break;
        }
        ans.push_back({pr[n][k][arr], pr[n][k][arr] + k - 1});
        int i = pr[n][k][arr];
        int idx1 = i, idx2 = i;
        for (int j = i; j < i + k; j++) {
            if (arr[j] < arr[idx1]) {
                idx1 = j;
            }
            if (arr[j] > arr[idx2]) {
                idx2 = j;
            }
        }
        // cout << "!" << i << ' ' << idx1 << ' ' << idx2 << endl;
        swap(arr[idx1], arr[idx2]);
    }
    // reverse(ans.begin(), ans.end());
    return ans;
}

int main(int argc, char* argv[]) {
    // registerGen(argc, argv, 1);

    for (int i = 1; i <= 7; i++) {
        for (int j = 1; j <= 7; j++) {
            pr[i][j] = {};
        }
    }
    for (int n = 1; n <= 7; n++) {
        for (int k = 1; k <= 7; k++) {
            vector<int> arr(n);
            for (int i = 0; i < n; i++) {
                arr[i] = i;
            }
            pr[n][k][arr] = -1;
            deque<vector<int>> dq;
            dq.push_back(arr);
            while (!dq.empty()) {
                vector<int> v = dq.front();
                dq.pop_front();
                for (int i = 0; i + k <= n; i++) {
                    vector<int> to = v;
                    int idx1 = i, idx2 = i;
                    for (int j = i; j < i + k; j++) {
                        if (v[j] < v[idx1]) {
                            idx1 = j;
                        }
                        if (v[j] > v[idx2]) {
                            idx2 = j;
                        }
                    }
                    swap(to[idx1], to[idx2]);
                    if (pr[n][k].find(to)  == pr[n][k].end()) {
                        pr[n][k][to] = i;
                        dq.push_back(to);
                    }
                }
            }
        }
    }
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
        if (n >= 2 * k - 1) {
            cout << "YES\n";
            vector<pair<int, int>> ans;
            vector<int> pos(n);
            for (int i = 0; i < n; i++) pos[arr[i]] = i;
            for (int i = 0; i + (2 * k - 1) < n; i++) {
                while (pos[i] >= i + 2 * k - 1) {
                    // for (auto el: arr) {
                    //     cout << el << ' ';
                    // }
                    // cout << '\n';
                    // cout << pos[i] << ' ' << i << endl;
                    int L = pos[i] - k + 1, R = pos[i];
                    ans.push_back({L, R});
                    
                    int idx1 = L, idx2 = L;
                    for (int j = L; j <= R; j++) {
                        if (arr[idx1] > arr[j]) {
                            idx1 = j;
                        }
                        if (arr[idx2] < arr[j]) {
                            idx2 = j;
                        }
                    }
                    swap(pos[arr[idx1]], pos[arr[idx2]]);
                    swap(arr[idx1], arr[idx2]);
                }
                if (pos[i] != i) {
                    vector<int> bbb;
                    for (int j = i; j < i + 2 * k - 1; j++) {
                        int cnt = 0;
                        for (int a = i; a < i + 2 * k - 1; a++) {
                            if (arr[a] < arr[j]) {
                                cnt++;
                            }
                        }
                        bbb.push_back(cnt);
                    }
                    // cout << '?';
                    // for (auto el: bbb) {
                    //     cout << el << ' ';
                    // }
                    // cout << '\n';

                    vector<pair<int, int>> tmp = sort(2 * k - 1, k, bbb);
                    for (auto el: tmp) {
                        // cout << '@' << el.first + 1 << ' ' << el.second + 1 << endl;
                        ans.push_back({el.first + i, el.second + i});
                    }
                    sort(arr.begin() + i , arr.begin() + i + 2 * k - 1);
                    for (int j = i; j < i + 2 * k - 1; j++) {
                        pos[arr[j]] = j;
                    }
                }
            }
            if (true) {
                vector<int> bbb;
                for (int j = n - (2 * k - 1); j < n; j++) {
                    int cnt = 0;
                    for (int a = n - (2 * k - 1); a < n; a++) {
                        if (arr[a] < arr[j]) {
                            cnt++;
                        }
                    }
                    bbb.push_back(cnt);
                }
                vector<pair<int, int>> tmp = sort(2 * k - 1, k, bbb);
                for (auto el: tmp) {
                    ans.push_back({el.first + n - (2 * k - 1), el.second + n - (2 * k - 1)});
                }
                sort(arr.begin() + n - (2 * k - 1) , arr.begin() + n);
            }
            cout << ans.size() << '\n';
            for (auto el: ans) {
                cout << el.first + 1 << ' ' << el.second + 1 << '\n';
            }
        } else {  
            if (pr[n][k].find(arr) == pr[n][k].end()) {
                cout << "NO\n";
                continue;
            }
            vector<pair<int, int>> ans = sort(n, k, arr);
            cout << "YES\n";
            cout << ans.size() << '\n';
            for (auto el: ans) {
                cout << el.first + 1 << ' ' << el.second + 1 << '\n'; 
            }
        }
    }
    return 0;
}