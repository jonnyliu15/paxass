#include <iostream>
#include <vector>

template <typename T> void printVector(std::vector<T> &someVector) {
    for (int i = 0; i < someVector.size(); i++) {
        std::cout << someVector[i] << " ";
    }
    std::cout << std::endl;
}
