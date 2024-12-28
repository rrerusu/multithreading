#include <iostream>
#include "SimpleLinkedListQueue.h"

int main() {
    Queue<int> myQueue = Queue<int>();

    myQueue.push(1);
    myQueue.push(1);
    myQueue.push(1);
    myQueue.push(1);
    myQueue.push(1);

    std::shared_ptr<int> valFromQueue = myQueue.try_pop();

    std::cout << *valFromQueue << std::endl;

    return 0;
}