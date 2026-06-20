#include <bits/stdc++.h>

using namespace std;

#define CNTG 500

mt19937 rng(42);

void play() {
    vector<int> ord = {1, 2, 3, 4, 5, 6};
    shuffle(ord.begin(), ord.end(), rng);
    for (int x: ord) {
        cout << x << endl;
        int mv; cin >> mv;
    }
}

int main() {
    for (int i = 0; i < CNTG; ++i)
        play();
}