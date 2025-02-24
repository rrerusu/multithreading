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

        std::atomic<unsigned> threadsInPop;
        std::atomic<Node * > toBeDeleted;

        static void deleteNodes(Node * nodes) {
            while(nodes) {
                Node * next = nodes->next;
                delete nodes;
                nodes = next;
            }
        }

        void tryReclaim(Node * oldHead) {
            if(threadsInPop == 1) {
                Node * nodesToDelete = toBeDeleted.exchange(nullptr);
                if(!--threadsInPop)
                    deleteNodes(nodesToDelete);
                else if(nodesToDelete)
                    chainPendingNodes(nodesToDelete);
                delete oldHead;
            } else {
                chainPendingNode(oldHead);
                --threadsInPop;
            }
        }

        void chainPendingNodes(Node * nodes) {
            Node * lastNode = nodes;
            while(Node * const nextNode = lastNode->next)
                lastNode = nextNode;
            chainPendingNodes(nodes, lastNode);
        }

        // TODO: Check this
        void chainPendingNodes(Node * firstNode, Node * lastNode) {
            lastNode->next = toBeDeleted;
            while(!toBeDeleted.compare_exchange_weak(lastNode->next, firstNode));
        }

        void chainPendingNode(Node * n) {
            chainPendingNodes(n, n);
        }

        std::atomic<Node * > head;
    public:
        void push(T const & data) {
            Node * const newNode = new Node(data);
            newNode->next = head.load();
            while(!head.compare_exchange_weak(newNode->next, newNode));
        }
        std::shared_ptr<T> pop() {
            ++threadsInPop;
            Node * oldHead = head.load();
            while(oldHead && !head.compare_exchange_weak(oldHead, oldHead->next));
            std::shared_ptr<T> res;
            if(oldHead)
                res.swap(oldHead->data);
            
            tryReclaim(oldHead);
            return res;
        }
};