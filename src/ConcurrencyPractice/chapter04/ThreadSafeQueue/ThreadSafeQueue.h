#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename someType>
class ThreadSafeQueue {
    public:
        ThreadSafeQueue();
        ThreadSafeQueue(const ThreadSafeQueue &);
        ThreadSafeQueue & operator=(const ThreadSafeQueue &) = delete;

        void push(someType newValue);

        bool tryPop(someType & value);
        std::shared_ptr<someType> tryPop();

        void waitAndPop(someType & value);
        std::shared_ptr<someType> waitAndPop();

        bool empty() const;
    private:
        mutable std::mutex dataMutex;
        std::queue<someType> dataQueue;
        std::condition_variable dataCondition;
};

template<typename someType>
ThreadSafeQueue<someType>::ThreadSafeQueue(const ThreadSafeQueue & other) {
    std::lock_guard<std::mutex> lk(other.dataMutex);
    dataQueue = other.dataQueue;
}

template<typename someType>
void ThreadSafeQueue<someType>::push(someType newValue) {
    std::lock_guard<std::mutex> lk(dataMutex);
    dataQueue.push(newValue);
    dataCondition.notify_one();
}

template<typename someType>
void ThreadSafeQueue<someType>::waitAndPop(someType & value) {
    std::unique_lock<std::mutex> lk(dataMutex);
    dataCondition.wait(lk, [this]{ return !dataQueue.empty(); });
    value = dataQueue.front();
    dataQueue.pop();
}

template<typename someType>
std::shared_ptr<someType> ThreadSafeQueue<someType>::waitAndPop() {
    std::unique_lock<std::mutex> lk(dataMutex);
    dataCondition.wait(lk, [this]{ return !dataQueue.empty(); });
    std::shared_ptr<someType> res(std::make_shared<someType>(dataQueue.front()));
    dataQueue.pop();
    return res;
}

template<typename someType>
bool ThreadSafeQueue<someType>::tryPop(someType & value) {
    std::lock_guard<std::mutex> lk(dataMutex);
    if(dataQueue.empty())
        return false;
    value = dataQueue.front();
    dataQueue.pop();
    return true;
}

template<typename someType>
std::shared_ptr<someType> ThreadSafeQueue<someType>::tryPop() {
    std::lock_guard<std::mutex> lk(dataMutex);
    if(dataQueue.empty())
        return std::shared_ptr<someType>();
    std::shared_ptr<someType> res(std::make_shared<someType>(dataQueue.front()));
    dataQueue.pop();
    return res;
}

template<typename someType>
bool ThreadSafeQueue<someType>::empty() const {
    std::lock_guard<std::mutex> lk(dataMutex);
    return dataQueue.empty();
}