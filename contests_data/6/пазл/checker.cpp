#include <bits/stdc++.h>
#include "testlib.h"

using namespace std;
using pi = pair<int, int>;

constexpr int T = 10'000;
constexpr int N = 1'000'000;
constexpr int M = 1'000'000;

const vector<vector<pi>> figures_shifts{
    {{0,0}, {0,1}, {1,0}, {1,1}},
    {{0,0}, {0,1}, {1,0}, {1,2}, {2,1}, {2,2}},
    {{0,0}, {0,1}, {1,-1}, {1,1}, {2,-1}, {2,0}}
};

bool CheckFigure(vector<pi> figure) {
    sort(figure.begin(), figure.end());
    for (const auto& standard_shifts : figures_shifts) {
        if (figure.size() != standard_shifts.size()) continue;

        bool ok = true;
        const auto [ax, ay] = figure[0];
        for (size_t i = 1; i != figure.size(); ++i) {
            const auto [bx, by] = figure[i];
            if (make_pair(bx-ax, by-ay) != standard_shifts[i])
                ok = false;
        }

        if (ok) return true;
    }
    return false;
}

bool ReadAnswer(InStream& stream, int n, int m, const vector<string>& v) {
    string answer = upperCase(stream.readWord("[a-zA-Z]+", "ans"));
    if (answer == "NO") return false;
    stream.quitif(answer != "YES", _wa, "unexpected answer, should be yes/no");

    vector<vector<int>> ans(n);
    for (auto& w : ans) w = stream.readInts(m, 0, 1'000'000'000, "w");

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            stream.quitif((v[i][j] == '.') != (ans[i][j] != 0), _wa, "isn't coverage");
        }
    }

    unordered_map<int, vector<pi>> figures;
    for (int i = 0; i < n; ++i)
    for (int j = 0; j < m; ++j) {
        if (ans[i][j]) figures[ans[i][j]].emplace_back(i, j);
    }

    for (const auto& [id, figure] : figures) {
        if (!CheckFigure(figure))
            stream.quitf(_wa, "figure #%d isn't allowed", id);
    }

    return true;
}

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    int t = inf.readInt(1, T, "t");
    for (int testcase = 1; testcase <= t; ++testcase) {
        setTestCase(testcase);

        int n = inf.readInt(2, N, "n");
        int m = inf.readInt(2, M, "m");
        string format = "[.#]{" + to_string(m) + "," + to_string(m) + "}";
        vector<string> v(n);
        for (auto& w : v) w = inf.readToken(format, "s");

        bool jans = ReadAnswer(ans, n, m, v);
        bool pans = ReadAnswer(ouf, n, m, v);

        if (jans && !pans) quitf(_wa, "participant hasn't found solution, but jury has");
        if (!jans && pans) quitf(_fail, "jury hasn't found solution, but participant has");
    }

    quitf(_ok, "you're awesome :3");
}