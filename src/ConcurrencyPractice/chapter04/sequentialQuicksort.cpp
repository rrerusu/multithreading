#include <iostream>
#include <list>
#include <algorithm>
#include <chrono>

template<typename T>
std::list<T> sequentialQuickSort(std::list<T> inputList) {
    if(inputList.empty()) {
        return inputList;
    }

    std::list<T> sortedList;
    sortedList.splice(sortedList.begin(), inputList, inputList.begin());
    T const & pivotValue = *sortedList.begin();

    // Get partition
    auto pivot = std::partition(inputList.begin(), inputList.end(), [&](T const & t){ return t < pivotValue; });

    std::list<T> leftSide;
    leftSide.splice(leftSide.end(), inputList, inputList.begin(), pivot);

    auto sortedLeftSide(sequentialQuickSort(std::move(leftSide)));
    auto sortedRightSide(sequentialQuickSort(std::move(inputList)));

    sortedList.splice(sortedList.end(), sortedRightSide);
    sortedList.splice(sortedList.begin(), sortedLeftSide);
    return sortedList;
}

int main() {
    std::list<int> listToSort;
    listToSort.push_back(12);
    listToSort.push_back(5);
    listToSort.push_back(8);
    listToSort.push_back(19);
    listToSort.push_back(2);
    listToSort.push_back(7);

    auto begin = std::chrono::system_clock::now();
    listToSort = sequentialQuickSort(listToSort);
    auto end = std::chrono::system_clock::now();

    for (int num : listToSort) {
        std::cout << num << " ";
    }

    std::printf("\nElapsed time: %d",
        std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());

    return EXIT_SUCCESS;
}