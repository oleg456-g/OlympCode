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
		long long j = ans.readLong();
        long long p = ouf.readLong();
        if (j == p) {
            score += 2;
        }
	}
	if (tests == 1) {
	    quitp(0, "sample ok");
	} else {
        quitp(score, "participant's solution is ok");
	}
    return 0;
}