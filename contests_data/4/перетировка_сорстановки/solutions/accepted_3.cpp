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

auto precalc(int32_t n, int32_t k) {
    std::queue<std::vector<int32_t>> active;
    std::map<std::vector<int32_t>, std::pair<int32_t, std::vector<int32_t>>> seen;

    std::vector<int32_t> start;
    for(int32_t i = 0; i < n; i++)
        start.push_back(i);
    active.push(start);
    while(!active.empty()) {
        std::vector<int32_t> arr = active.front();
        active.pop();

        for(int32_t i = 0; i + k - 1 < n; i++) {
            int32_t ind_min = i, ind_max = i;
            for(int32_t j = i + 1; j < i + k; j++) {
                if(arr[j] > arr[ind_max])
                    ind_max = j;
                if(arr[j] < arr[ind_min])
                    ind_min = j;
            }

            std::vector<int32_t> arr2 = arr;
            std::swap(arr2[ind_min], arr2[ind_max]);
            if(seen.find(arr2) == seen.end()) {
                seen[arr2] = {i, arr};
                active.push(arr2);
            }
        }
    }
    return seen;
}

int main() {
    auto steps_k_3 = precalc(5, 3);
    auto steps_k_4_n_5 = precalc(5, 4);
    auto steps_k_4_n_6 = precalc(6, 4);
    auto steps_k_4_n_7 = precalc(7, 4);

    /*int32_t n = 8, k = 4;

    std::queue<std::vector<int32_t>> active;
    std::set<std::vector<int32_t>> seen;

    std::vector<int32_t> start;
    for(int32_t i = 0; i < n; i++)
        start.push_back(i);
    active.push(start);
    while(!active.empty()) {
        std::vector<int32_t> arr = active.front();
        active.pop();

        for(int32_t i = 0; i + k - 1 < n; i++) {
            int32_t ind_min = i, ind_max = i;
            for(int32_t j = i + 1; j < i + k; j++) {
                if(arr[j] > arr[ind_max])
                    ind_max = j;
                if(arr[j] < arr[ind_min])
                    ind_min = j;
            }

            std::vector<int32_t> arr2 = arr;
            std::swap(arr2[ind_min], arr2[ind_max]);
            if(seen.find(arr2) == seen.end()) {
                seen.insert(arr2);
                active.push(arr2);
            }
        }
    }

    for(const auto& arr : seen) {
        for(int32_t i = 0; i < n; i++)
            std::cout << arr[i] << " ";
        std::cout << "\n";
    }
    std::cout << seen.size() << "\n";
    return 0;*/

    std::vector<int32_t> start_5;
    for(int32_t i = 0; i < 5; i++)
        start_5.push_back(i);
    std::vector<int32_t> start_6;
    for(int32_t i = 0; i < 6; i++)
        start_6.push_back(i);
    std::vector<int32_t> start_7;
    for(int32_t i = 0; i < 7; i++)
        start_7.push_back(i);

    int32_t num_tests;
    std::cin >> num_tests;

    for(int32_t t = 0; t < num_tests; t++) {
        int32_t n, k;
        std::cin >> n >> k;

        int32_t* arr = new int32_t[n];
        for(int32_t i = 0; i < n; i++)
            std::cin >> arr[i];

        auto do_op = [&](int32_t i) {
            int32_t ind_min = i, ind_max = i;
            for(int32_t j = i + 1; j < i + k; j++) {
                if(arr[j] > arr[ind_max])
                    ind_max = j;
                if(arr[j] < arr[ind_min])
                    ind_min = j;
            }

            std::swap(arr[ind_min], arr[ind_max]);
        };

        std::vector<int32_t> ops;
        bool ok = true;
        if(k == 2) {
            for(int32_t i = 0; i < n; i++)
                for(int32_t j = 0; j + 1 < n - i; j++)
                    if(arr[j + 1] < arr[j]) {
                        ops.push_back(j);
                        std::swap(arr[j + 1], arr[j]);
                    }
        } else if(k == 3) {
            int32_t perm2[5];
            int32_t perm[5];
            for(int32_t i = 0; i < 5; i++)
                perm2[i] = i;
            for(int32_t i = 0; i < n; i++)
                for(int32_t j = 0; j + 4 < n; j++) {
                    std::sort(perm2, perm2 + 5, [&](int32_t ind1, int32_t ind2) {
                        return arr[j + ind1] < arr[j + ind2];
                    });
                    for(int32_t p = 0; p < 5; p++)
                        perm[perm2[p]] = p;

                    std::vector<int32_t> cur(perm, perm + 5);
                    while(cur != start_5) {
                        ops.push_back(j + steps_k_3[cur].first);
                        do_op(ops.back());
                        cur = steps_k_3[cur].second;
                    }
                }
        } else if(k == 4 && n == 5) {
            int32_t perm2[5];
            int32_t perm[5];
            for(int32_t i = 0; i < 5; i++)
                perm2[i] = i;
            for(int32_t i = 0; i < n && ok; i++)
                for(int32_t j = 0; j + 4 < n; j++) {
                    std::sort(perm2, perm2 + 5, [&](int32_t ind1, int32_t ind2) {
                        return arr[j + ind1] < arr[j + ind2];
                    });
                    for(int32_t p = 0; p < 5; p++)
                        perm[perm2[p]] = p;

                    std::vector<int32_t> cur(perm, perm + 5);
                    if(steps_k_4_n_5.find(cur) == steps_k_4_n_5.end()) {
                        std::cout << "NO" << "\n";
                        ok = false;
                        break;
                    }

                    while(cur != start_5) {
                        ops.push_back(j + steps_k_4_n_5[cur].first);
                        do_op(ops.back());
                        cur = steps_k_3[cur].second;
                    }
                }
        } else if(k == 4 && n == 6) {
            int32_t perm2[6];
            int32_t perm[6];
            for(int32_t i = 0; i < 6; i++)
                perm2[i] = i;
            for(int32_t i = 0; i < n && ok; i++)
                for(int32_t j = 0; j + 5 < n; j++) {
                    std::sort(perm2, perm2 + 6, [&](int32_t ind1, int32_t ind2) {
                        return arr[j + ind1] < arr[j + ind2];
                    });
                    for(int32_t p = 0; p < 6; p++)
                        perm[perm2[p]] = p;

                    std::vector<int32_t> cur(perm, perm + 6);
                    if(steps_k_4_n_6.find(cur) == steps_k_4_n_6.end()) {
                        std::cout << "NO" << "\n";
                        ok = false;
                        break;
                    }

                    while(cur != start_6) {
                        ops.push_back(j + steps_k_4_n_6[cur].first);
                        do_op(ops.back());
                        cur = steps_k_4_n_6[cur].second;
                    }
                }
        } else {
            int32_t perm2[7];
            int32_t perm[7];
            for(int32_t i = 0; i < 7; i++)
                perm2[i] = i;
            for(int32_t i = 0; i < n; i++)
                for(int32_t j = 0; j + 6 < n; j++) {
                    std::sort(perm2, perm2 + 7, [&](int32_t ind1, int32_t ind2) {
                        return arr[j + ind1] < arr[j + ind2];
                    });
                    for(int32_t p = 0; p < 7; p++)
                        perm[perm2[p]] = p;

                    std::vector<int32_t> cur(perm, perm + 7);
                    while(cur != start_7) {
                        ops.push_back(j + steps_k_4_n_7[cur].first);
                        do_op(ops.back());
                        cur = steps_k_4_n_7[cur].second;
                    }
                }
        }

        if(ok) {
            std::cout << "YES" << "\n";
            std::cout << ops.size() << "\n";
            for(int32_t i = 0; i < ops.size(); i++)
                std::cout << ops[i] + 1 << " " << ops[i] + k << "\n";

            /*std::cout << "+ ";
            for(int32_t i = 0; i < n; i++)
                std::cout << arr[i] << " ";
            std::cout << "\n";*/
        }
    }
}





