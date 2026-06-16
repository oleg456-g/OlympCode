#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cmath>
#include <string>
#include <deque>
typedef long long ll;
ll inf = 1000000000000000000;
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, a;
    cin >> n;
    deque <int> h1, h2;
    char s;
    vector <int> ans;
    for (int i = 0; i < n; i++) {
        cin >> s;
        if (s == '+') {
            cin >> a;
            h2.push_back(a);
            if (h2.size() > h1.size()) {
                h1.push_back(h2.front());
                h2.pop_front();
            }
        } else if (s == '*') {
            cin >> a;
            if (h1.size() == h2.size()) {
                h1.push_back(a);
            } else {
                h2.push_front(a);
            }
        } else if (s == '-') {
            ans.push_back(h1.front());
            h1.pop_front();
            if (h2.size() > h1.size()) {
                h1.push_back(h2.front());
                h2.pop_front();
            }
        }
    }
    for (int i = 0; i < int(ans.size()); i++) {
        cout << ans[i] << endl;
    }
}