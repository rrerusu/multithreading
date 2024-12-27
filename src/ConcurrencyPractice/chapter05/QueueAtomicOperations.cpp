#include <atomic>
#include <thread>
#include <vector>

std::vector<int> queueData;
std::atomic<int> count;

void populateQueue() {
    unsigned const numberOfItems = 20;
    queueData.clear();
    for(unsigned i = 0; i < numberOfItems; ++i)
        queueData.push_back(i);
    count.store(numberOfItems, std::memory_order_release);
}

void consumeQueueItems() {
    while(true) {
        int itemIndex = 0;
        if((itemIndex = count.fetch_sub(1, std::memory_order_acquire)) <= 0) {
            waitForMoreItems();
            continue;
        }
        process(queueData[itemIndex - 1]);
    }
}

int main() {
    std::thread a(populateQueue);
    std::thread b(consumeQueueItems);
    std::thread c(consumeQueueItems);
    a.join();
    b.join();
    c.join();

    return 0;
}