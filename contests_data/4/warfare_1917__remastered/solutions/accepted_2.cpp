#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
using namespace std;

bool check1(vector<int> arr) {
    vector<int> p = {1, 2, 3, 4, 5, 6};
    return (p == arr);
}

bool check2(vector<int> arr) {
    vector<int> p = {6, 5, 4, 3, 2, 1};
    return (p == arr);
}

bool check3(vector<int> arr) {
    sort(arr.begin(), arr.begin() + 3);
    sort(arr.begin() + 3, arr.end());
    vector<int> p = {1, 2, 3, 4, 5,  6};
    return (p == arr);
}

bool check4(vector<int> arr) {
    sort(arr.rbegin(), arr.rbegin() + 3);
    sort(arr.rbegin() + 3, arr.rend());
    vector<int> p = {6, 5, 4, 3, 2, 1};
    return (p == arr);
}

bool check5(vector<int> arr) {
    vector<int> p = {1, 2, 3, 4, 5, 6};
    for (auto el: arr) {
        if (el == p[0]) {
            p.erase(p.begin());
        } else if (el == p[1]) {
            p.erase(p.begin() + 1);
        } else {
            return false;
        }
    }
    return true;
}

bool check6(vector<int> arr) {
    vector<int> p = {6, 5, 4, 3, 2, 1};
    for (auto el: arr) {
        if (el == p[0]) {
            p.erase(p.begin());
        } else if (el == p[1]) {
            p.erase(p.begin() + 1);
        } else {
            return false;
        }
    }
    return true;
}

bool check7(vector<int> arr) {
    vector<int> p = {1, 2, 3, 4, 5, 6};
    for (auto el: arr) {
        if (el == p[0]) {
            p.erase(p.begin());
        } else if (el == p[1]) {
            p.erase(p.begin() + 1);
        } else if (el == p[2]) {
            p.erase(p.begin() + 2);
        } else {
            return false;
        }
    }
    return true;
}

bool check8(vector<int> arr) {
    vector<int> p = {6, 5, 4, 3, 2, 1};
    for (auto el: arr) {
        if (el == p[0]) {
            p.erase(p.begin());
        } else if (el == p[1]) {
            p.erase(p.begin() + 1);
        } else if (el == p[2]) {
            p.erase(p.begin() + 1);
        } else {
            return false;
        }
    }
    return true;
}

bool check9(vector<int> arr) {
    vector<int> p = {6, 5, 4, 3, 2, 1};
    for (auto el: arr) {
        if (el == p[0]) {
            p.erase(p.begin());
        } else if (el == p[1]) {
            p.erase(p.begin() + 1);
        } else if (el == p[2]) {
            p.erase(p.begin() + 2);
        } else {
            return false;
        }
    }
    return true;
}

int main() {
    int n = 6;
    vector<int> cnts(8);
    int X = 30;
    vector<vector<int>> EEE;
    for (int _ = 0; _ < X; _++) {
        vector<int> arr(n);
        iota(arr.begin(), arr.end(), 1);
        vector<int> tmp;
        for (auto el: arr) {
            cout << el << endl;
            int x;
            cin >> x;
            tmp.push_back(x);
        }
        EEE.push_back(tmp);
        cnts[0] += check1(tmp);
        cnts[1] += check2(tmp);
        cnts[2] += check3(tmp);
        cnts[3] += check4(tmp);
        cnts[4] += check5(tmp);
        cnts[5] += check6(tmp);
        cnts[6] += check7(tmp);
        cnts[7] += check9(tmp);
    }
    // int idx = max_element(cnts.begin(), cnts.end()) - cnts.begin();
    int idx = 0;
    for (int i  = 0; i < 8; i++) {
        if (cnts[i] > cnts[idx]) {
            idx = i;
        }
    }
    vector<vector<int>> opts = {
        {2, 3, 4, 5, 6, 1},
        {1, 6, 5, 4, 3, 2}, 
        {3, 4, 5, 6, 1, 2},
        {1, 2, 6, 5, 4, 3},
        {3, 4, 5, 6, 1, 2},
        {1, 2, 6, 5, 4, 3},
        {4, 3, 5, 6, 1, 2},
        {2, 1, 6, 3, 4, 5}};
    if (idx == 7) {
        cnts.assign(8,0);
        for (auto tmp: EEE) {
            cnts[0] += check1(tmp);
            cnts[1] += check2(tmp);
            cnts[2] += check3(tmp);
            cnts[3] += check4(tmp);
            cnts[4] += check5(tmp);
            cnts[5] += check6(tmp);
            cnts[6] += check7(tmp);
            cnts[7] += check8(tmp);
        }
        idx = 0;
        for (int i  = 0; i < 8; i++) {
            if (cnts[i] > cnts[idx]) {
                idx = i;
            }
        }
        opts = {
            {2, 3, 4, 5, 6, 1},
            {1, 6, 5, 4, 3, 2}, 
            {3, 2, 1, 6, 5, 4},
            {6, 5, 4, 3, 2, 1},
            {3, 4, 5, 6, 1, 2},
            {1, 2, 6, 5, 4, 3},
            {3, 4, 5, 6, 1, 2},
            {1, 2, 3, 5, 4, 3}};   
    }
    for (int _ = 0; _ < 500 - X; _++) {
        for (auto el: opts[idx]) {
            cout << el << endl;
            int x;
            cin >> x;   
        }
    }
}