#include "macros.h"

#include <iostream>
#include <vector>
#include <algorithm>

class LazySelector {
public:
    LazySelector(const std::vector<int>& data) : data(data) {}

    int get(int index) const {
        if (index < 0 || index >= data.size()) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    int size() const {
        return data.size();
    }

private:
    std::vector<int> data;
};

int main() {
    std::vector<int> data = {1, 2, 3, 4, 5};
    LazySelector selector(data);

    std::cout << "Element at index 2: " << selector.get(2) << std::endl;
    std::cout << "Size of the selector: " << selector.size() << std::endl;

    return 0;
} 