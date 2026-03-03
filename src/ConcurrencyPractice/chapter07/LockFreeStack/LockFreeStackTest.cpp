#include <iostream>
#include <vector>

#include "LockFreeStack.h"

LockFreeStack<int> myStack;

void producer(int id) {
    for (int i = 0; i < 1000; i++) {
        myStack.push(id * 1000 + i);
    }
}

void consumer() {
    for (int i = 0; i < 1000; i++) {
        auto val = myStack.pop();
        if(val)
            std::cout << * val << "\n";
    }
}


int main() {
    std::vector<std::thread> threads;

   for (int i = 0; i < 4; i++)
        threads.emplace_back(producer, i);

    // Start consumers
    for (int i = 0; i < 4; i++)
        threads.emplace_back(consumer);
    
    for (auto &t : threads)
        t.join();

    return EXIT_SUCCESS;
}