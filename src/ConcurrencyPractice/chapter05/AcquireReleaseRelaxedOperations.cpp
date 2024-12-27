#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void writeXThenY() {
    x.store(true, std::memory_order_relaxed);
    y.store(true, std::memory_order_release);
}

void readYThenX() {
    while(!y.load(std::memory_order_acquire));
    if(x.load(std::memory_order_relaxed))
        ++z;
}

int main() {
    x = false, y = false;
    z = 0;
    std::thread a(writeXThenY);
    std::thread b(readYThenX);
    a.join();
    b.join();

    assert(z.load() != 0);

    return 0;
}