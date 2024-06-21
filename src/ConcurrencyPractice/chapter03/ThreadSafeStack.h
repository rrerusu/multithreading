#include <exception>
#include <memory>
#include <mutex>
#include <stack>

struct emptyStackException : std::exception {
    const char * what() {
        return "This stack is empty.";
    }
};

template<typename T>
class ThreadSafeStack {
    private:
        std::stack<T> data;
        mutable std::mutex stackMutex;
    public:
        ThreadSafeStack() {}
        ThreadSafeStack(const ThreadSafeStack &);
        ThreadSafeStack & operator=(const ThreadSafeStack &) = delete;

        void push(T val);
        std::shared_ptr<T> pop();
        void pop(T & val);
        bool empty() const;
};

template<typename T>
ThreadSafeStack<T>::ThreadSafeStack(const ThreadSafeStack & anotherStack) {
    std::lock_guard<std::mutex> otherStackLock(anotherStack.stackMutex);
    data = anotherStack.data;
}

template<typename T>
void ThreadSafeStack<T>::push(T val) {
    std::lock_guard<std::mutex> stackLock(stackMutex);
    data.push(val);
}

template<typename T>
std::shared_ptr<T> ThreadSafeStack<T>::pop() {
    std::lock_guard<std::mutex> stackLock(stackMutex);
    if(data.empty())
        throw emptyStackException();
    std::shared_ptr<T> const ptrToTop(std::make_shared<T>(data.top()));
    data.pop();
    return ptrToTop;
}

template<typename T>
void ThreadSafeStack<T>::pop(T & val) {
    std::lock_guard<std::mutex> stackLock(stackMutex);
    if(data.empty())
        throw emptyStackException();
    val = data.top();
    data.pop();
}

template<typename T>
bool ThreadSafeStack<T>::empty() const {
    std::lock_guard<std::mutex> stackLock(stackMutex);
    return data.empty();
}