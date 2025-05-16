#include "macros.h"

#include <iostream>
#include <vector>
#include <algorithm>

class LazySelector {
public:
    LazySelector(const std::vector<int>& data) : data(data) {}

    int select(int k) {
        if (k < 0 || k >= data.size()) {
            throw std::out_of_range("Index out of range");
        }
        return data[k];
    }

private:
    std::vector<int> data;
};

int main() {
    std::vector<int> data = {1, 2, 3, 4, 5};
    LazySelector selector(data);

    std::cout << "Selected element: " << selector.select(2) << std::endl;

    return 0;
} 