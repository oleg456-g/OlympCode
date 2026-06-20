#include "testlib.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <set>

using namespace std;

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);
	double score = 0;
	int tests = inf.readInt();
	for (int testno = 0; testno < tests; ++testno) {
		double part = ouf.readDouble();
		double jury = ans.readDouble();
		if (part == jury) score += 5;
	}
    quitp(score, "participant's solution is ok");
    return 0;
}