#include <queue>
#include <condition_variable>
#include <mutex>
#include <memory>

#pragma once

template<typename F>
class ThreadSafeQueue {
    public:
        ThreadSafeQueue();
        void push(F item) {
            std::lock_guard<std::mutex> guard(queueMutex);
            queue.push(std::move(item));
            conditionalAccess.notify_one();
        }
        void wait_and_pop(F& item) {
            std::unique_lock<std::mutex> locke(queueMutex);
            conditionalAccess.wait(locke, [this]{ return !queue.empty(); });
            F value = std::move(queue.front());
            queue.pop();
        }
        std::shared_ptr<F> wait_and_pop() {
            std::unique_lock<std::mutex> locke(queueMutex);
            conditionalAccess.wait(locke, [this]{ return !queue.empty(); });
            std::shared_ptr<F> res(std::make_shared<F>(std::move(queue.front())));
            queue.pop();
            return res;
        }
        bool try_pop(F& value) {
            std::lock_guard<std::mutex> guard(queueMutex);
            if(queue.empty())
                return false;
            F value = std::move(queue.front());
            queue.pop();
            return true;
        }

        std::shared_ptr<F> try_pop() {
            std::lock_guard<std::mutex> guard(queueMutex);
            if(queue.empty())
                return false;
            std::shared_ptr<F> res(std::make_shared<F>(std::move(queue.front())));
            queue.pop();
            return res;
        }

        bool empty() const {
            std::lock_guard<std::mutex> guard(queueMutex);
            return queue.empty();
        }
    private:
        std::queue<F> queue;
        mutable std::mutex queueMutex;
        std::condition_variable conditionalAccess;
};

