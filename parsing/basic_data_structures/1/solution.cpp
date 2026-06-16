#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stack>

using namespace std;

int main() {
    char k;
    bool flag;
    stack<char> a;
    flag = true;
    while (cin >> k) {
        if (a.empty()) {
            a.push(k);
        } else if ((k == ')' && a.top() != '(') || (k == ']' && a.top() != '[') || (k == '}' && a.top() != '{')) {
            cout << "no";
            flag = false;
            break;
        } else if (k == '(' || k == '[' || k == '{') {
            a.push(k);
        } else {
            a.pop();
        }
    }

    if (!a.empty() && flag) {
        cout << "no";
    } else if (a.empty()) {
        cout << "yes";
    }
}