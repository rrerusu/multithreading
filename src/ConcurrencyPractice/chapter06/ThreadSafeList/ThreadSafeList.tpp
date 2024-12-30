#include "ThreadSafeList.h"

template<typename T>
ThreadSafeList<T>::~ThreadSafeList() {
    ThreadSafeList<T>::remove_if([](Node const &){ return true; });
}

// Challenge method
template<typename T>
void ThreadSafeList<T>::insert(T const & value, int const & index) {
    Node * current = & head;
    std::unique_lock<std::mutex> currentNodeLock(head.nodeTex);
    
    int counter = 0;
    Node * next = current->next.get();
    while(counter < index && next != nullptr) {
        std::unique_lock<std::mutex> nextNodeLock(next->nodeTex);
        currentNodeLock.unlock();

        counter++;
        current = next;

        currentNodeLock = std::move(nextNodeLock);

        next = next->next.get();
    }

    if(index > counter)
        throw std::out_of_range("Index " + std::to_string(index) + " is out of bounds.");

    std::unique_ptr<Node> newNode(new Node(value));
    newNode->next = std::move(current->next);
    current->next = std::move(newNode);
}

template<typename T>
void ThreadSafeList<T>::push_front(T const & value) {
    std::unique_ptr<Node> newNode(new Node(value));
    std::lock_guard<std::mutex> headLock(head.nodeTex);

    newNode->next = std::move(head.next);
    head.next = std::move(newNode);
}

template<typename T>
template<typename Function>
void ThreadSafeList<T>::for_each(Function Func) {
    Node * current = & head;
    std::unique_lock<std::mutex> currentNodeLock(head.nodeTex);

    while(Node * const next = current->next.get()) {
        std::unique_lock<std::mutex> nextNodeLock(next->nodeTex);
        currentNodeLock.unlock();

        Func( * next->data);
        current = next;

        currentNodeLock = std::move(nextNodeLock);
    }
}

template<typename T>
template<typename Predicate>
std::shared_ptr<T> ThreadSafeList<T>::find_first_if(Predicate p) {
    Node * current = & head;
    std::unique_lock<std::mutex> currentNodeLock(head.nodeTex);

    while(Node * const next = current->next.get()) {
        std::unique_lock<std::mutex> nextNodeLock(next->nodeTex);
        currentNodeLock.unlock();

        if(p( * next->data))
            return next->data;
        current = next;

        currentNodeLock = std::move(nextNodeLock);
    }
    return std::shared_ptr<T>();
}

template<typename T>
template<typename Predicate>
void ThreadSafeList<T>::remove_if(Predicate p) {
    Node * current = & head;
    std::unique_lock<std::mutex> currentNodeLock(head.nodeTex);

    while(Node * const next = current->next.get()) {
        std::unique_lock<std::mutex> nextNodeLock(next->nodeTex);

        if(p(*next->data)) {
            std::unique_ptr<Node> oldNext = std::move(current->next);
            current->next = std::move(next->next);

            nextNodeLock.unlock();
        } else {
            currentNodeLock.unlock();

            current = next;

            currentNodeLock = std::move(nextNodeLock);
        }
    }
}