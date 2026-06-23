#include "bits/stdc++.h"
#include "testlib.h"

using namespace std;

int main(int argc, char *argv[]) {
    registerTestlibCmd(argc, argv);
    int score = 0;
    int tests = inf.readInt();

    for (int test_index = 1; test_index <= tests; test_index++) {
        setTestCase(test_index);
        int p_ans = ouf.readInt(0, INT_MAX, "pans");
        int jury_ans = ans.readInt(0, INT_MAX, "jans");
        if (p_ans == jury_ans) {
            score += 5;
        }
    }

    quitp(score, "participant's solution is ok");
    return 0;
}
