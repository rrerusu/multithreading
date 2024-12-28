#include <memory>
#include <mutex>
#include <condition_variable>

#pragma once

template<typename T>
class Queue {
    private:
        struct Node {
            std::shared_ptr<T> data;
            std::unique_ptr<Node> next;

            Node() = default;
            Node(T data_): data(std::make_shared<T>(std::move(data_))) {}
        };

        std::mutex headTex;
        std::unique_ptr<Node> head;
        std::mutex tailTex;
        Node * tail;
        std::condition_variable queueCondVar;

        Node * getTail();
        std::unique_ptr<Node> popHead();
        std::unique_lock<std::mutex> waitForData();
        std::unique_ptr<Node> waitPopHead();
        std::unique_ptr<Node> waitPopHead(T &);
        std::unique_ptr<Node> tryPopHead();
        std::unique_ptr<Node> tryPopHead(T &);


    public:
        Queue() : head(new Node), tail(head.get()) {}
        Queue(const Queue &) = delete;
        Queue(Queue &&) noexcept;
        Queue & operator=(const Queue &) = delete;
        Queue & operator=(Queue &&) noexcept;
        ~Queue() noexcept = default;

        std::shared_ptr<T> wait_and_pop();
        void wait_and_pop(T &);
        std::shared_ptr<T> try_pop();
        bool try_pop(T &);
        void push(T);
        bool empty() const;
};

#include "SimpleLinkedListQueue.tpp"