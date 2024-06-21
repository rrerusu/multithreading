#include <iostream>
#include "ThreadSafeStack.h"

int main() {
    ThreadSafeStack<int> mySafeStack;

    mySafeStack.push(1);
    mySafeStack.push(2);
    mySafeStack.push(3);
    mySafeStack.push(4);
    mySafeStack.push(5);
    mySafeStack.push(6);
    mySafeStack.push(7);
    mySafeStack.push(8);
    mySafeStack.push(9);

    while(!mySafeStack.empty()) {
        std::cout << *(mySafeStack.pop()) << std::endl;
    }

    return EXIT_SUCCESS;
}