#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stack>


using namespace std;


int main() {
    vector<string> a;
    string s;
    while (cin >> s) {
        if (s.empty()) {
            break;
        }
        a.push_back(s);
    }
    for (int i = 0; i < a.size(); i++) {
        for (int j = i; j > 0 && a[j] + a[j - 1] > a[j - 1] + a[j]; j--) {
            swap(a[j], a[j - 1]);
        }
    }
    for (int i = 0; i < a.size(); i++) {
        cout << a[i];
    }
}