#include <vector>
#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

std::vector<int> data;
std::atomic<bool> dataReady(false);
std::mutex dataMutex;

void readerThread() {
    while(!dataReady.load())
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::lock_guard<std::mutex> dataLock(dataMutex);
    std::cout << "The answer = " << data[0] << std::endl;
}

void writerThread() {
    {
        std::lock_guard<std::mutex> dataLock(dataMutex);
        data.push_back(42);
    }
    dataReady = true;
}

int main() {
    std::thread t1(&readerThread);
    std::thread t2(&writerThread);

    t1.join();
    t2.join();

    return EXIT_SUCCESS;
}