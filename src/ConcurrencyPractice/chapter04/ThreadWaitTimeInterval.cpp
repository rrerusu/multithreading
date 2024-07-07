#include <thread>
#include <mutex>

bool waitFlag;
std::mutex m;

void waitForFlag() {
    std::unique_lock<std::mutex> lk(m);
    while(!waitFlag) {
        lk.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        lk.lock();
    }
}