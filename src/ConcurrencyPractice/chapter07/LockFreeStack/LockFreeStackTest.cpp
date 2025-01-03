#include <iostream>

#include "LockFreeStack.h"

int main() {
    LockFreeStack<int> myStack;

    myStack.push(10);

    std::cout << *(myStack.pop()) << std::endl;

    return EXIT_SUCCESS;
}