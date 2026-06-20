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
		int n = inf.readInt();
        int m = inf.readInt();
        int k = inf.readInt();
        for (int j = 0; j < m; j++) {
            int a = inf.readInt();
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                int a = inf.readInt();
            }
        }
        for (int i = 0; i < k; i++) {
            int a = inf.readInt();
        }
        bool flag = true;
        for (int i = 0; i < n; i++) {
            int correct = ans.readInt();
            int participant = ouf.readInt();
            if (correct != participant) {
                flag = false;
            }
        }
        if (flag) score += 2;
	}
    quitp(score, "participant's solution is ok");
    return 0;
}