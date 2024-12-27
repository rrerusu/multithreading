#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <assert.h>

struct X {
    int i;
    std::string s;
};

std::atomic<X *> p;
std::atomic<int> a;

void createX() {
    X * x = new X;
    x->i = 42;
    x->s = "Hello";
    a.store(99, std::memory_order_relaxed);
    p.store(x, std::memory_order_release);
}

void useX() {
    X * x;
    while(!(x = p.load(std::memory_order_consume)))
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    assert(x->i == 42);
    assert(x->s == "Hello");
    assert(a.load(std::memory_order_relaxed) == 99);
}

int main() {
    std::thread t1(createX);
    std::thread t2(useX);
    t2.join();
    t1.join();

    return 0;
}