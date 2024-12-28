#include <memory>

template<typename T>
class Queue {
    private:
        struct Node {
            T data;
            std::unique_ptr<Node> next;

            Node(T data_): data(std::move(data_)) {}
        };

        std::unique_ptr<Node> head;
        Node * tail;
    public:
        Queue() = default;
        Queue(const Queue & other) = noexcept delete;
        Queue(Queue && other) {
            head = std::move(other.head);
            tail = std::move(other.tail);
        }
        Queue & operator=(const Queue & other) = delete;
        Queue & operator=(Queue && other) noexcept {
            head = std::move(other.head);
            tail = std::move(other.tail);
        }
        ~Queue() noexcept = default;

        std::shared_ptr<T> try_pop() {
            if(!head)
                return std::shared_ptr<T>();
            
            std::shared_ptr<T> const result(std::make_shared<T>(std::move(head->data)));
            std::unique_ptr<Node> const oldHead = std::move(head);
            head = std::move(oldHead->next);
            return result;
        }
        void push(T newVal) {
            std::unique_ptr<Node> p(new Node(std::move(newVal)));
            Node * const newTail = p.get();
            if(tail)
                tail->next = std::move(p);
            else
                head = std::move(p);
            
            tail = newTail;
        }
};