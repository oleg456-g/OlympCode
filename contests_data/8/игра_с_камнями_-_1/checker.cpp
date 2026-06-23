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
	int t = inf.readInt();
	for (int i = 0; i < t; ++i) {
		long long j = ans.readLong();
		long long p = ouf.readLong();
		if (j == p) score += 10;
	}
    quitp(score, "participant's solution is ok");
    return 0;
}