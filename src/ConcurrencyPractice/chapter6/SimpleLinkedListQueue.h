#include <memory>
#include <mutex>

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

        Node * getTail() {
            std::lock_guard<std::mutex> tailLock(tailTex);
            return tail;
        }
        std::unique_ptr<Node> popHead() {
            std::lock_guard<std::mutex> head_lock(headTex);

            if(head.get() == getTail())
                return nullptr;
            
            std::unique_ptr<Node> oldHead = std::move(head);
            head = std::move(oldHead->next);
            return oldHead;
        }

    public:
        Queue() : head(new Node), tail(head.get()) {}
        Queue(const Queue & other) = noexcept delete;
        Queue(Queue && other) {
            std::lock(other.headTex, other.tailTex);
            std::lock_guard<std::mutex> otherHeadLock(other.headTex, std::adopt_lock);
            std::lock_guard<std::mutex> otherTailLock(other.tailTex, std::adopt_lock);
            head = std::move(other.head);
            tail = std::move(other.tail);
        }
        Queue & operator=(const Queue & other) = delete;
        Queue & operator=(Queue && other) noexcept {
            std::lock(other.headTex, other.tailTex);
            std::lock_guard<std::mutex> otherHeadLock(other.headTex, std::adopt_lock);
            std::lock_guard<std::mutex> otherTailLock(other.tailTex, std::adopt_lock);
            std::lock(headTex, tailTex);
            std::lock_guard<std::mutex> headLock(headTex, std::adopt_lock);
            std::lock_guard<std::mutex> tailLock(tailTex, std::adopt_lock);
            head = std::move(other.head);
            tail = std::move(other.tail);
        }
        ~Queue() noexcept = default;

        std::shared_ptr<T> try_pop() {
            std::unique_ptr<Node> oldHead = popHead();
            return oldHead ? oldHead->data : std::shared_ptr<T>();
        }
        void push(T newVal) {
            std::shared_ptr<T> newData(std::make_shared<T>(std::move(newVal)));
            std::unique_ptr<Node> p(new Node());
            Node * const newTail = p.get();

            std::lock_guard<std::mutex> tailLock(tailTex);
            tail->data = newData;
            tail->next = std::move(p);
            tail = newTail;
        }
};