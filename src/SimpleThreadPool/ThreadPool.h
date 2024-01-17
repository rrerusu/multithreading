#include <atomic>
#include <queue>
#include <functional>
#include <vector>
#include <thread>
#include "ThreadSafeQueue.h"
#include "JoinThreads.h"

#pragma once

class ThreadPool {
    public:
        ThreadPool() : done(false), joiner(threads) {
            unsigned const int maxThreads = std::thread::hardware_concurrency();
            try {
                for(unsigned int i = 0; i < maxThreads; i++)
                    threads.push_back(std::thread(&ThreadPool::workerThread));
            } catch(...) {
                done = true;
            }
        }

        template<typename F>
        void submit(F func) {
            jobs.push(std::function<void()>(func));
        }
    private:
        std::atomic_bool done;
        ThreadSafeQueue<std::function<void()>> jobs;
        std::vector<std::thread> threads;
        JoinThreads joiner;
        void workerThread() {
            while(!done) {
                std::function<void()> task;
                if(jobs.try_pop(task))
                    task();
                else
                    std::this_thread::yield();
            }
        }
};