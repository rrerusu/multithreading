#include <condition_variable>
#include <mutex>
#include <chrono>
#include <iostream>

std::condition_variable waitConditionVar;
bool isDone;                    // Some operation we want to pass to a loop
std::mutex waitMutex;

bool waitLoop() {
    auto const timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(5000);
    std::unique_lock<std::mutex> waitLock(waitMutex);
    while(!isDone) {
        if(waitConditionVar.wait_until(waitLock, timeout) == std::cv_status::timeout)
            break;
    }
    return isDone;
}

int main() {
    bool test = waitLoop();
    
    return EXIT_SUCCESS;
}