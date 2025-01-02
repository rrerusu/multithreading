#include <atomic>

template<typename T>
class LockFreeStack {
    private:
        struct Node {
            T data;
            Node * next;

            Node(T const & data_) : data(data_) {}
        };

        std::atomic<Node * > head;
    public:
        void push(T const & data) {
            Node * const newNode = new Node(data);
            newNode->next = head.load();
            while(!head.compare_exchange_weak(newNode->next, newNode));
        }
};