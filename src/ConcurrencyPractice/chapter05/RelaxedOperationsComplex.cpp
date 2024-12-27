#include <thread>
#include <atomic>
#include <iostream>

std::atomic<int> x(0), y(0), z(0);
std::atomic<bool> go(false);

unsigned const loopCount = 10;
struct readValues {
    int x, y, z;
};

readValues values1[loopCount];
readValues values2[loopCount];
readValues values3[loopCount];
readValues values4[loopCount];
readValues values5[loopCount];

void increment(std::atomic<int> * varToInc, readValues * values) {
    while(!go)
        std::this_thread::yield();
    for(unsigned i = 0; i < loopCount; ++i) {
        values[i].x = x.load(std::memory_order_relaxed);
        values[i].y = y.load(std::memory_order_relaxed);
        values[i].z = z.load(std::memory_order_relaxed);
        varToInc->store(i + 1, std::memory_order_relaxed);
        std::this_thread::yield();
    }
}

void readVals(readValues * values) {
    while(!go)
        std::this_thread::yield();
    for(unsigned i = 0; i < loopCount; ++i) {
        values[i].x = x.load(std::memory_order_relaxed);
        values[i].y = y.load(std::memory_order_relaxed);
        values[i].z = z.load(std::memory_order_relaxed);
        std::this_thread::yield();
    }
}

void print(readValues * v) {
    for(unsigned i = 0; i < loopCount; ++i) {
        if(i)
            std::cout << ", ";
        std::cout << "(" << v[i].x << ", " << v[i].y << ", " << v[i].z << ")";
    }
    std::cout << std::endl;
}

int main() {
    std::thread t1(increment, &x, values1);
    std::thread t2(increment, &y, values2);
    std::thread t3(increment, &z, values3);
    std::thread t4(readVals, values4);
    std::thread t5(readVals, values5);

    go = true;

    t5.join();
    t4.join();
    t3.join();
    t2.join();
    t1.join();

    print(values1);
    print(values2);
    print(values3);
    print(values4);
    print(values5);

    return EXIT_SUCCESS;
}