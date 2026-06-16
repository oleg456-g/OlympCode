#include <iostream>

int main() {
    int n;
    std::cin >> n;
    int x = 0, y = 0;
    for (int i = 0; i < n; ++i) {
        int curr;
        std::cin >> curr;
        if (curr > 0) {
            x += curr;
        } else {
            y -= curr;
        }
    }
    std::cout << std::max(x, y) << std::endl;
}
