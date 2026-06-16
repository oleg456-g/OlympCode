#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    string s, k;
    cin >> s >> k;
    sort(k.begin(), k.end());
    sort(s.begin(), s.end());
    if (k == s) {
        cout << "YES";
    } else {
        cout << "NO";
    }
}