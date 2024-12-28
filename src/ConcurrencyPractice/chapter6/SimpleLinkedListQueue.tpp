#include "SimpleLinkedListQueue.h"

// Private Member Functions
template<typename T>
typename Queue<T>::Node * Queue<T>::getTail() {
    std::lock_guard<std::mutex> tailLock(tailTex);
    return tail;
}

template<typename T>
std::unique_ptr<typename Queue<T>::Node> Queue<T>::popHead() {
    std::unique_ptr<typename Queue<T>::Node> oldHead = std::move(head);
    head = std::move(oldHead->next);
    return oldHead;
}

template<typename T>
std::unique_lock<std::mutex> Queue<T>::waitForData() {
    std::unique_lock<std::mutex> headLock(headTex);
    queueCondVar.wait(headLock, [&](){ return head.get() != getTail(); });
    return std::move(headLock);
}

template<typename T>
std::unique_ptr<typename Queue<T>::Node> Queue<T>::waitPopHead() {
    std::unique_lock<std::mutex> headLock(waitForData());
    return popHead();
}

template<typename T>
std::unique_ptr<typename Queue<T>::Node> Queue<T>::waitPopHead(T & val) {
    std::unique_lock<std::mutex> headLock(waitForData());
    val = std::move(*head->data);
    return popHead();
}

template<typename T>
std::unique_ptr<typename Queue<T>::Node> Queue<T>::tryPopHead() {
    std::lock_guard<std::mutex> headLock(headTex);
    if(head.get() == getTail())
        return std::unique_ptr<typename Queue<T>::Node>();
    
    return popHead();
}

template<typename T>
std::unique_ptr<typename Queue<T>::Node> Queue<T>::tryPopHead(T & val) {
    std::lock_guard<std::mutex> headLock(headTex);
    if(head.get() == getTail())
        return std::unique_ptr<typename Queue<T>::Node>();
    
    val = std::move(*head->data);
    return popHead();
}





// Public Member Functions

template<typename T>
Queue<T>::Queue(Queue && other) noexcept {
    std::lock(other.headTex, other.tailTex);
    std::lock_guard<std::mutex> otherHeadLock(other.headTex, std::adopt_lock);
    std::lock_guard<std::mutex> otherTailLock(other.tailTex, std::adopt_lock);

    head = std::move(other.head);
    tail = std::move(other.tail);
}

template<typename T>
Queue<T> & Queue<T>::operator=(Queue<T> && other) noexcept {
    std::lock(other.headTex, other.tailTex);
    std::lock_guard<std::mutex> otherHeadLock(other.headTex, std::adopt_lock);
    std::lock_guard<std::mutex> otherTailLock(other.tailTex, std::adopt_lock);

    std::lock(headTex, tailTex);
    std::lock_guard<std::mutex> headLock(headTex, std::adopt_lock);
    std::lock_guard<std::mutex> tailLock(tailTex, std::adopt_lock);
    
    head = std::move(other.head);
    tail = std::move(other.tail);
}

template<typename T>
std::shared_ptr<T> Queue<T>::wait_and_pop() {
    std::unique_ptr<Node> const oldHead = waitPopHead();
    return oldHead->data;
}

template<typename T>
void Queue<T>::wait_and_pop(T & val) {
    std::unique_ptr<Node> const oldHead = waitPopHead(val);
}

template<typename T>
std::shared_ptr<T> Queue<T>::try_pop() {
    std::unique_ptr<Node> oldHead = tryPopHead();
    return oldHead ? oldHead->data : std::shared_ptr<T>();
}

template<typename T>
bool Queue<T>::try_pop(T & val) {
    std::unique_ptr<Node> const oldHead = tryPopHead(val);
    return oldHead;
}

template<typename T>
void Queue<T>::push(T newVal) {
    std::shared_ptr<T> newData(std::make_shared<T>(std::move(newVal)));
    std::unique_ptr<typename Queue<T>::Node> p(new typename Queue<T>::Node());
    {
        std::lock_guard<std::mutex> tailLock(tailTex);    
        tail->data = newData;
        Node * const newTail = p.get();
        tail->next = std::move(p);
        tail = newTail;
    }

    queueCondVar.notify_one();
}

template<typename T>
bool Queue<T>::empty() const {
    std::lock_guard<std::mutex> headLock(headTex);
    return head.get() == getTail();
}