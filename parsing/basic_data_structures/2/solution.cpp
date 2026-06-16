#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cmath>
#include <string>
typedef long long ll;
ll inf = 1000000000000000000;
using namespace std;
int main() {
    string a;
    vector <int> s;
    int cnt = -1;
    int res;
    while (cin >> a) {
        if (a != "+" && a != "-" && a != "*") {
            s.push_back(stoi(a));
            cnt++;
        } else if (a == "+") {
            res = s[cnt-1]+s[cnt];
            s.pop_back();
            s.pop_back();
            s.push_back(res);
            cnt--;
        } else if (a == "-") {
            res = s[cnt-1]-s[cnt];
            s.pop_back();
            s.pop_back();
            s.push_back(res);
            cnt--;
        } else if (a == "*") {
            res = s[cnt-1]*s[cnt];
            s.pop_back();
            s.pop_back();
            s.push_back(res);
            cnt--;
        }
    }
    cout << s[0];
}