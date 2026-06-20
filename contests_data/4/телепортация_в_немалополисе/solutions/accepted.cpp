#include <cstdint>
#include <iostream>
#include <cmath>
#include <cctype>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <deque>
#include <stack>
#include <unordered_set>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <queue>
#include <unordered_map>
#include <random>
#include <cfloat>
#include <chrono>
#include <bitset>
#include <complex>
#include <functional>
#include <immintrin.h>
#include <memory>

#define BASE 97
#define MOD 1'000'000'007

int32_t* base_powers;

struct Node {
    int32_t children[26];
    int32_t hash;
    int32_t depth;

    Node() {
        for(int32_t i = 0; i < 26; i++)
            children[i] = -1;
        hash = 0;
        depth = 0;
    }
};

#define INF 1'000'000'000'000'000'000ll

int main() {
    base_powers = new int32_t[300'005];
    base_powers[0] = 1;
    for(int32_t i = 1; i < 300'005; i++)
        base_powers[i] = ((int64_t)base_powers[i - 1] * BASE) % MOD;

    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    int32_t num_tests;
    std::cin >> num_tests;

    for(int32_t t = 0; t < num_tests; t++) {
        int32_t n, k;
        std::cin >> n >> k;

        std::string* arr = new std::string[n];
        int32_t max_len = 0;
        for(int32_t i = 0; i < n; i++) {
            std::cin >> arr[i];
            max_len = std::max(max_len, (int32_t)arr[i].size());
        }

        std::vector<Node> nodes;
        nodes.emplace_back();
        std::map<int32_t, int32_t>* node_per_hash = new std::map<int32_t, int32_t>[max_len + 1];
        for(int32_t i = 0; i < n; i++) {
            int32_t v = 0;
            for(int32_t j = 0; j < arr[i].size(); j++) {
                if(nodes[v].children[arr[i][j] - 'a'] == -1) {
                    nodes.emplace_back();
                    nodes[v].children[arr[i][j] - 'a'] = nodes.size() - 1;
                    nodes.back().hash = ((int64_t)nodes[v].hash * BASE + arr[i][j] - 'a' + 1) % MOD;
                    nodes.back().depth = nodes[v].depth + 1;
                }
                v = nodes[v].children[arr[i][j] - 'a'];
                node_per_hash[j + 1][nodes[v].hash] = v;
            }
        }

        std::vector<std::pair<int32_t, int32_t>>* graph = new std::vector<std::pair<int32_t, int32_t>>[n + 4 * nodes.size()];

        // edges from suffixes
        for(int32_t i = 0; i < n; i++) {
            bool all_same = true;
            int32_t hash = 0;
            for(int32_t j = 1; j <= arr[i].size(); j++) {
                hash += ((int64_t)(arr[i][arr[i].size() - j] - 'a' + 1) * base_powers[j - 1]) % MOD;
                hash %= MOD;
                auto it = node_per_hash[j].find(hash);
                if(it != node_per_hash[j].end()) {
                    if(all_same && j < arr[i].size() && arr[i][arr[i].size() - j] == arr[i][arr[i].size() - j - 1])
                        graph[i].emplace_back(n + 3 * nodes.size() + it->second, 0);
                    else
                        graph[i].emplace_back(n + 2 * nodes.size() + it->second, 0);
                }
                if(j < arr[i].size())
                    all_same = all_same && (arr[i][arr[i].size() - j] == arr[i][arr[i].size() - j - 1]);
            }
        }

        // edges from prefixes
        for(int32_t i = 0; i < n; i++) {
            bool all_same = true;
            int32_t v = 0;
            for(int32_t j = 1; j <= arr[i].size(); j++) {
                v = nodes[v].children[arr[i][j - 1] - 'a'];

                if(all_same && j < arr[i].size() && arr[i][j] == arr[i][j - 1])
                    graph[n + nodes.size() + v].emplace_back(i, 0);
                else
                    graph[n + v].emplace_back(i, 0);

                if(j < arr[i].size())
                    all_same = all_same && (arr[i][j] == arr[i][j - 1]);
            }
        }

        // transits
        for(int32_t i = 0; i < nodes.size(); i++) {
            if(nodes[i].depth >= k) {
                // normal -> normal
                graph[n + 2 * nodes.size() + i].emplace_back(n + i, nodes[i].depth);
                // normal -> hazard
                graph[n + 2 * nodes.size() + i].emplace_back(n + nodes.size() + i, nodes[i].depth);
                // hazard -> normal
                graph[n + 3 * nodes.size() + i].emplace_back(n + i, nodes[i].depth);
            }
        }

        int64_t* dist = new int64_t[n + 4 * nodes.size()];
        for(int32_t i = 0; i < n + 4 * nodes.size(); i++)
            dist[i] = INF;
        dist[0] = 0;

        std::set<std::pair<int64_t, int32_t>> active;
        active.emplace(0, 0);
        while(!active.empty()) {
            int32_t v = active.begin()->second;
            active.erase(active.begin());

            for(int32_t i = 0; i < graph[v].size(); i++)
                if(dist[graph[v][i].first] > dist[v] + graph[v][i].second) {
                    auto it = active.find({dist[graph[v][i].first], graph[v][i].first});
                    if(it != active.end())
                        active.erase(it);
                    dist[graph[v][i].first] = dist[v] + graph[v][i].second;
                    active.emplace(dist[graph[v][i].first], graph[v][i].first);
                }
        }

        for(int32_t i = 1; i < n; i++)
            std::cout << ((dist[i] == INF) ? -1 : dist[i]) << " ";
        std::cout << "\n";
    }
}

