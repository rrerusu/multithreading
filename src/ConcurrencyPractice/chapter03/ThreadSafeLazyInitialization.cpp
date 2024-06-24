#include <mutex>
#include <memory>
#include <iostream>

struct someResource {
    void doSomething() {
        std::cout << "Doing something";
    }
};

std::shared_ptr<someResource> resourcePtr;
std::mutex resourceMutex;

void foo() {
    std::unique_lock<std::mutex> resourceLock(resourceMutex);
    if(!resourcePtr)
        resourcePtr.reset(new someResource);
    resourceLock.unlock();
    resourcePtr->doSomething();
}

// Double cbecked locking causes undefined behavior - nasty data race condition
void doubleCheckedLocking() {
    if(!resourcePtr) {
        std::lock_guard<std::mutex> resourceLock(resourceMutex);
        if(!resourcePtr)
            resourcePtr.reset(new someResource);
    }
    resourcePtr->doSomething();
}