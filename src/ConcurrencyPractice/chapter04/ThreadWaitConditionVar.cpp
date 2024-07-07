#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>

struct dataChunk{};
bool moreDataToPrepare();
dataChunk prepareData();
void process(dataChunk someData);
bool isLastChunk(dataChunk someData);




std::mutex mut;
std::queue<dataChunk> dataQueue;
std::condition_variable dataCondition;

void dataPreparationThread() {
    while(moreDataToPrepare()) {
        const dataChunk data = prepareData();
        std::lock_guard<std::mutex> lk(mut);
        dataQueue.push(data);
        dataCondition.notify_one();
    }
}

void dataProcessingThread() {
    while(true) {
        std::unique_lock<std::mutex> lk(mut);
        dataCondition.wait(lk, []{ return !dataQueue.empty(); });
        dataChunk data = dataQueue.front();
        dataQueue.pop();
        lk.unlock();
        process(data);
        if(isLastChunk(data))
            break;
    }
}