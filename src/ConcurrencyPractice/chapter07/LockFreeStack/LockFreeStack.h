#include <atomic>
#include <memory>

template<typename T>
class LockFreeStack {
    private:
        struct Node {
            std::shared_ptr<T> data;
            Node * next;

            Node(T const & data_) : data(std::make_shared<T>(data_)) {}
        };

        std::atomic<Node * > head;
    public:
        void push(T const & data) {
            Node * const newNode = new Node(data);
            newNode->next = head.load();
            while(!head.compare_exchange_weak(newNode->next, newNode));
        }
        std::shared_ptr<T> pop() {
            Node * oldHead = head.load();
            while(oldHead && !head.compare_exchange_weak(oldHead, oldHead->next));
            return oldHead ? oldHead->data : std::shared_ptr<T>();
        }
};