#include <exception>
#include <memory>
#include <mutex>
#include <stack>
#include <string>

struct emptyStack: std::exception {
    std::string message;

    explicit emptyStack(const std::string & msg) : message(msg) {}

    const char * what() const noexcept override {
        return message.c_str();
    }
};

template<typename T>
class threadSafeStack {
    private:
        std::stack<T> stackData;
        mutable std::mutex stackTex;
    public:
        threadSafeStack() = default;
        threadSafeStack(const threadSafeStack & other) {
            std::lock_guard<std::mutex> stackLock(other.stackTex);
            stackData = other.stackData;
        }
        threadSafeStack(ThreadSafeStack && other) noexcept {
            std::lock_guard<std::mutex> stackLock(other.stackTex);
            stackData = std::move(other.stackData);
        }
        threadSafeStack & operator=(const threadSafeStack &) = delete;
        threadSafeStack & operator=(threadSafeStack && other) noexcept {
            if(this != &other) {
                std::lock(stackTex, other.stackTex);
                std::lock_guard<std::mutex> otherStackLock(other.stackTex, std::adopt_lock);
                std::lock_guard<std::mutex> stackLock(stackTex, std::adopt_lock);
                stackData = std::move(other.stackData);
            }
            return *this;
        }
        ~threadSafeStack() noexcept = default;

        void push(T newVal) {
            std::lock_guard<std::mutex> stackLock(stackTex);
            stackData.push(std::move(newVal));
        }
        std::shared_ptr<T> pop() {
            std::lock_guard<std::mutex> stackLock(stackTex);
            if(stackData.empty())
                throw emptyStack("Stack is empty");
            
            std::shared_ptr<T> const result(std::make_shared<T>(std::move(stackData.top())));
            stackData.pop();
            return result;
        }
        void pop(T & value) {
            std::lock_guard<std::mutex> stackLock(stackTex);
            if(stackData.empty())
                throw emptyStack("Stack is empty");
            
            value = std::move(stackData.top());
            stackData.pop();
        }
        bool empty() const {
            std::lock_guard<std::mutex> stackLock(stackTex);
            return stackData.empty();
        }
};