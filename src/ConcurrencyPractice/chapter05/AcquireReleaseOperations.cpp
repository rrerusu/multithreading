#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void writeX() {
    x.store(true, std::memory_order_release);
}

void writeY() {
    y.store(true, std::memory_order_release);
}

void readXThenY() {
    while(!x.load(std::memory_order_acquire));
    if(y.load(std::memory_order_acquire))
        ++z;
}

void readYThenX() {
    while(!y.load(std::memory_order_acquire));
    if(x.load(std::memory_order_acquire))
        ++z;
}

int main() {
    x = false, y = false;
    z = 0;

    std::thread a(writeX);
    std::thread b(writeY);
    std::thread c(readXThenY);
    std::thread d(readYThenX);

    a.join();
    b.join();
    c.join();
    d.join();

    assert(z.load() != 0);

    return 0;
}