#include <mutex>

void prepareData();

std::unique_lock<std::mutex> getLock() {
    extern std::mutex someMutex;
    std::unique_lock<std::mutex> someLock(someMutex);
    prepareData();
    return someLock;
}
void processData() {
    std::unique_lock<std::mutex> lk(getLock());
    // Do something
}