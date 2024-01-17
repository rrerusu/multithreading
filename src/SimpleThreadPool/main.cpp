#include "ThreadPool.h"
#include <iostream>
#include <future>

int main() {
    ThreadPool myPool();
    std::vector<std::future<void()>> futures;
    futures.push_back(myPool.submit(std::bind([](){ return 0; })));

    return EXIT_SUCCESS;
}