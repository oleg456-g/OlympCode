#include "testlib.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <set>

using namespace std;

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);
    int score = 0;
    int tests = inf.readInt();
    for (int testno = 0; testno < tests; ++testno) {
        int correct = ans.readInt();
        int participant = ouf.readInt();

        if (correct == participant) {
            score += 1;
        }
    }
    quitp(score, "participant's solution is ok");
    return 0;
}