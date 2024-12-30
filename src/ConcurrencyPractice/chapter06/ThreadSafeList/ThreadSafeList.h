#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

#pragma once

template<typename T>
class ThreadSafeList {
    struct Node {
        std::mutex nodeTex;
        std::shared_ptr<T> data;
        std::unique_ptr<Node> next;
        Node() : next() {}
        Node(T const & value) : data(std::make_shared<T>(value)) {}
    };

    private:
        Node head;
    public:
        ThreadSafeList() {}
        ThreadSafeList(ThreadSafeList const &) = delete;
        ThreadSafeList & operator=(ThreadSafeList const &) = delete;
        ~ThreadSafeList() ;

        void insert(T const &, int const &);
        void push_front(T const &);

        template<typename Function>
        void for_each(Function);
        
        template<typename Predicate>
        std::shared_ptr<T> find_first_if(Predicate);
        
        template<typename Predicate>
        void remove_if(Predicate);
};

#include "ThreadSafeList.tpp"