#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cmath>
#include <string>
#include <queue>
typedef long long ll;
ll inf = 1000000000000000000;
using namespace std;
int main() {
    int n;
    cin >> n;
    vector <float> swim(n);
    float s = 0;
    for (int i = 0; i < n; i++) {
        cin >> swim[i];
        s += swim[i];
    }
    sort(swim.begin(), swim.end(), greater<float>());
    int k;
    cin >> k;
    vector <float> proc;
    vector<float> ch;
    int cl;
    for (int i = 0; i < k; i++) {
        cin >> cl;
        if (cl == 1) {
            cin >> cl;
            proc.push_back(cl);
        } else {
            cin >> cl;
            ch.push_back(cl);
        }
    }
    sort(ch.begin(), ch.end(), greater<float>());
    sort(proc.begin(), proc.end(), greater<float>());
    while (int(proc.size()) > n) {
        proc.pop_back();
    }

    while (int(ch.size()) > n) {
        ch.pop_back();
    }
    vector <float> d;
    for (int i = 0; i < int(proc.size()); i++) {
        d.push_back(swim[i] * proc[i] / 100);
    }
    for (int i = 0; i < int(ch.size()); i++) {
        d.push_back(ch[i]);
    }
    sort(d.begin(), d.end(), greater<float>());
    for (int i = 0; i < min(n, int(d.size())); i++) {
        s += d[i];
    }
    cout << s;
}