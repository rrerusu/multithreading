#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <functional>

void doWork(unsigned id) {
    std::cout << id << std::endl;
}

void f() {
    std::vector<std::thread> threads;
    for(unsigned i = 0; i < 20; i++)
        threads.push_back(std::thread(doWork, i));
    
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}

int main() {
    f();
}