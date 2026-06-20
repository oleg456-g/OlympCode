#include <bits/stdc++.h>
#include <functional>
using namespace std;

#define CNTG 500

mt19937 rng(42);

const auto enmin = {1, 2, 3, 4, 5, 6};
const auto enmax = {6, 5, 4, 3, 2, 1};

int cntmin, cntmax, cntrmin, cntrmax, cntr2min, cntr2max, cntr3min, cntr3max;

void play() {
    vector<int> ord = enmin;
    shuffle(ord.begin(), ord.end(), rng);
    vector<int> en;
    for (int x: ord) {
        cout << x << endl;
        int mv; cin >> mv;
        cerr << mv << ' ';
        en.push_back(mv);
    }
    cerr << endl;
    cntmin += equal(en.begin(), en.end(), enmin.begin());
    cntmax += equal(en.begin(), en.end(), enmax.begin());
    {
        vector<int> en3 = en;
        sort(en3.begin(), en3.begin() + 3);
        sort(en3.begin() + 3, en3.end());
        cntrmin += equal(en3.begin(), en3.end(), enmin.begin());
        cntrmax += equal(en3.begin(), en3.begin() + 3, enmin.begin() + 3) && equal(en3.begin() + 3, en3.end(), enmin.begin());
    }
    {
        bool f = true;
        vector<int> mi2 = enmin;
        for (int x: en) {
            if (mi2.size() == 1) break;
            if (mi2[0] == x) {
                mi2.erase(mi2.begin());
            } else if (mi2[1] == x) {
                mi2.erase(mi2.begin() + 1);
            } else {
                f = false;
                break;
            }
        }
        cntr2min += f;
    }
    {
        bool f = true;
        vector<int> ma2 = enmax;
        for (int x: en) {
            if (ma2.size() == 1) break;
            if (ma2[0] == x) {
                ma2.erase(ma2.begin());
            } else if (ma2[1] == x) {
                ma2.erase(ma2.begin() + 1);
            } else {
                f = false;
                break;
            }
        }
        cntr2max += f;
    }
    {
        bool f = true;
        vector<int> mi3 = enmin;
        for (int x: en) {
            if (mi3.size() == 1) break;
            if (mi3[0] == x) {
                mi3.erase(mi3.begin());
            } else if (mi3[1] == x) {
                mi3.erase(mi3.begin() + 1);
            } else if (mi3[2] == x) {
                mi3.erase(mi3.begin() + 2);
            } else {
                f = false;
                break;
            }
        }
        cntr3min += f;
    }
    {
        bool f = true;
        vector<int> ma3 = enmax;
        for (int x: en) {
            if (ma3.size() == 1) break;
            if (ma3[0] == x) {
                ma3.erase(ma3.begin());
            } else if (ma3[1] == x) {
                ma3.erase(ma3.begin() + 1);
            } else if (ma3[2] == x) {
                ma3.erase(ma3.begin() + 2);
            } else {
                f = false;
                break;
            }
        }
        cntr3max += f;
    }
}

void play_min() {
    vector<int> ord = {2, 3, 4, 5, 6, 1};
    for (int x: ord) {
        cout << x << endl;
        int mv; cin >> mv;
    }
}

void play_max() {
    vector<int> ord = {1, 6, 5, 4, 3, 2};
    for (int x: ord) {
        cout << x << endl;
        int mv; cin >> mv;
    }
}

void play_rmin() {
    vector<int> ord = {2, 3, 4, 1, 5, 6};
    for (int x: ord) {
        cout << x << endl;
        int mv; cin >> mv;
    }
}

void play_rmax() {
    vector<int> ord = {1, 5, 6, 2, 3, 4};
    for (int x: ord) {
        cout << x << endl;
        int mv; cin >> mv;
    }
}

void play_r2min() {
    vector<int> ord = {3, 4, 5, 6, 2, 1};
    for (int x: ord) {
        cout << x << endl;
        int mv; cin >> mv;
    }
}

void play_r2max() {
    vector<int> ord = {1, 2, 6, 5, 4, 3};
    for (int x: ord) {
        cout << x << endl;
        int mv; cin >> mv;
    }
}

void play_r3min() {
    vector<int> ord = {3, 4, 5, 1, 2, 6};
    for (int x: ord) {
        cout << x << endl;
        int mv; cin >> mv;
    }
}

void play_r3max() {
    vector<int> ord = {1, 6, 5, 2, 4, 3};
    for (int x: ord) {
        cout << x << endl;
        int mv; cin >> mv;
    }
}

int main() {
    for (int i = 0; i < 10; ++i)
        play();
    function<void()> fplay;
    if (cntmin == 10) {
        fplay = play_min;
    } else if (cntmax == 10) {
        fplay = play_max;
    } else if (cntrmin == 10) {
        fplay = play_rmin;
    } else if (cntrmax == 10) {
        fplay = play_rmax;
    } else if (cntr2min == 10) {
        fplay = play_r2min;
    } else if (cntr2max == 10) {
        fplay = play_r2max;
    } else if (cntr3min == 10) {
        fplay = play_r3min;
    } else if (cntr3max == 10) {
        fplay = play_r3max;
    }
    cerr << "max:" << cntmax << " min:" << cntmin \
         << " rmin:" << cntrmin << " rmax:" << cntrmax \
         << " r2min:" << cntr2min << " r2max:" << cntr2max \
         << " r3min:" << cntr3min << " r3max:" << cntr3max \
         << endl;
    for (int i = 10; i < CNTG; ++i)
        fplay();
}