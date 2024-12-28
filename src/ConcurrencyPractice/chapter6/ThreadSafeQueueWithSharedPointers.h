#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <shared_motex>

template<typename T>
class ThreadSafeQueue {
    private:
        mutable std::mutex queueTex;
        std::queue<std::shared_ptr<T>> queueData;
        std::condition_variable queueConditionVar;
    public:
        ThreadSafeQueue() = default;
        ThreadSafeQueue(const ThreadSafeQueue & other) {
            std::lock_guard<std::mutex> otherLockGuard(other.queueTex);
            queueData = other.queueData;
        }
        ThreadSafeQueue(ThreadSafeQueue && other) noexcept {
            std::lock_guard<std::mutex> otherLockGuard(other.queueTex);
            queueData = std::move(other.queueData);
        }
        ThreadSafeQueue & operator=(ThreadSafeQueue & other) = delete;
        ThreadSafeQueue & operator=(ThreadSafeQueue && other) noexcept {
            if(this != &other) {
                std::lock(queueTex, other.queueTex);
                std::lock_guard<std::mutex> queueLock(queueTex, std::adopt_lock);
                std::lock_guard<std::mutex> otherQueueLock(other.queueTex, std::adopt_lock);
                queueData = std::move(other.queueData);
            }
            return *this;
        }
        ~ThreadSafeQueue() noexcept = default;

        void push(T newVal) {
            std::shared_ptr<T> data(std::make_shared<T>(std::move(newVal)));
            std::lock_guard<std::mutex> queueLock(queueTex);
            queueData.push(data);
            queueConditionVar.notify_one();
        }
        void waitAndPop(T& value) {
            std::unique_lock<std::mutex> uniqueQueueLock(queueTex);
            queueConditionVar.wait(uniqueQueueLock, [this](){ return !queueData.empty(); });
            value = std::move(* queueData.front());
            queueData.pop();
        }
        std::shared_ptr<T> waitAndPop() {
            std::unique_lock<std::mutex> uniqueQueueLock(queueTex);
            queueConditionVar.wait(uniqueQueueLock, [this](){ return !queueData.empty(); });
            std::shared_ptr<T> result(queueData.front());

            queueData.pop();
            return result;
        }
        bool tryPop(T & val) {
            std::lock_guard<std::mutex> queueLock(queueTex);
            if(queueData.empty())
                return false;
            
            val = std::move(* queueData.front());
            queueData.pop();
            return true;
        }
        std::shared_ptr<T> tryPop() {
            std::lock_guard<std::mutex> queueLock(queueTex);
            if(queueData.empty())
                return std::shared_ptr<T>();
            
            std::shared_ptr<T> result(queueData.front());

            queueData.pop();
            return result;
        }
        bool empty() const {
            std::shared_lock<std::mutex> queueLock(queueTex);
            return queueData.empty();
        }
};