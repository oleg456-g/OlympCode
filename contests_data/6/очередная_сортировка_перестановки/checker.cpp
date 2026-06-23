#include <iostream>
#include <vector>
#include "testlib.h"
 
using namespace std;
 
bool find_solution(int n, vector<int> p, InStream &in, bool jury) {
    int q = in.readInt(-1, 4 * n, "q");
    if (q == -1)
        return false;
    for (int i = 0; i < q; ++i) {
        int a = in.readInt(1, n, "i");
        int b = in.readInt(1, n, "j");
        if (a == b) {
            in.quitf(_wa, "i shouldn't be equal j");
        }
        --a, --b;
        if (p[a] != 1) {
            in.quitf(_wa, "p[i] should be equal 1");
        }
        if (abs(a - b) > n / 2) {
            in.quitf(_wa, "|a - b| shouldn't be more then n/2");
        }
        swap(p[a], p[b]);
    }
    for (int i = 0; i < n; ++i) {
        if (p[i] != i + 1) {
            in.quitf(_wa, "after all operation p should be sorted");
        }
    }
    return true;
}
 
int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);
 
    int test_count = inf.readInt();
    for (int q = 0; q < test_count; ++q) {
        setTestCase(q + 1);
        int n = inf.readInt();
        vector<int> p(n);
        for (int i = 0; i < n; ++i) {
            p[i] = inf.readInt();
        }
 
        bool ans1 = find_solution(n, p, ouf, 0);
        bool ans2 = find_solution(n, p, ans, 1);
        if (ans1 != ans2) {
            ouf.quitf(_wa, "Jury found the solution, but patricional not");
        }
    }
    quitf(_ok, "You are so good");
}
