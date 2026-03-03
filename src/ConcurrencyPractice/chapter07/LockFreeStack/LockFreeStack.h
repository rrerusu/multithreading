#include <memory>

#include "HazardPointers.h"

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
            std::atomic<void * > & hazardPointer = getHazardPointerForCurrentThread();
            Node * oldHead = head.load();
            do {
                Node * temp;
                do {
                    temp = oldHead;
                    hazardPointer.store(oldHead);
                    oldHead = head.load();
                } while(oldHead != temp);
            } while(oldHead && !head.compare_exchange_strong(oldHead, oldHead->next));

            hazardPointer.store(nullptr);
            std::shared_ptr<T> res;

            if(oldHead) {
                res.swap(oldHead->data);
                if(outstandingHazardPointersFor(oldHead))
                    reclaimLater(oldHead);
                else
                    delete oldHead;
                deleteNodesWithNoHazards();
            }
            return res;
        }


        // reclaim functions
        std::atomic<dataToReclaim * > nodesToReclaim;

        struct dataToReclaim {
            void * data;
            std::function<void(void * )> deleter;
            dataToReclaim * next;

            // template<typename T>
            dataToReclaim(T * p) :
                data(p),
                deleter( & doDelete<T>),
                next(0)
            {}

            ~dataToReclaim() {
                deleter(data);
            }
        };

        // template<typename T>
        void doDelete(void * p) {
            delete static_cast<T * >(p);
        }

        void addToReclaimList(aToReclaim * node) {
            node->next = nodesToReclaim.load();
            while(!nodesToReclaim.compare_exchange_weak(node->next, node));
        }

        void reclaimLater(T * data) {
            addToReclaimList(new dataToReclaim(data));
        }

        void deleteNodesWithNoHazards() {
            dataToReclaim * current = nodesToReclaim.exchange(nullptr);
            while(current) {
                dataToReclaim * const next = current->next;
                if(!outstandingHazardPointersFor(current->data))
                    delete current;
                else
                    addToReclaimList(current);
                current = next;
            }
        }
};