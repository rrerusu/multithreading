#include <mutex>
#include <stdexcept>

class HierarchicalMutex {
    private:
        std::mutex internalMutex;
        unsigned long const hierarchyValue;
        unsigned long previousHierarchyValue;
        static thread_local unsigned long thisThreadHierarchyValue;

        void checkForHierarchyViolation() {
            if(thisThreadHierarchyValue <= hierarchyValue)
                throw std::logic_error("Mutex hierarchy violated");
        }
        void updateHierarchyValue() {
            previousHierarchyValue = thisThreadHierarchyValue;
            thisThreadHierarchyValue = hierarchyValue;
        }
    public:
        explicit HierarchicalMutex(unsigned long val) :
            hierarchyValue(val), previousHierarchyValue(0) {}
        void lock() {
            checkForHierarchyViolation();
            internalMutex.lock();
            updateHierarchyValue();
        }
        void unlock() {
            thisThreadHierarchyValue = previousHierarchyValue;
            internalMutex.unlock();
        }
        bool try_lock() {
            checkForHierarchyViolation();
            if(!internalMutex.try_lock())
                return false;
            updateHierarchyValue();
            return true;
        }
};

thread_local unsigned long HierarchicalMutex::thisThreadHierarchyValue(ULONG_MAX);