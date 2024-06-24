#include <mutex>
#include <memory>
#include <iostream>

struct someResource {
    void doSomething() {
        std::cout << "Doing something";
    }
};

std::shared_ptr<someResource> resourcePtr;
// std::mutex resourceMutex;
std::once_flag resourceFlag;


// void foo() {
//     std::unique_lock<std::mutex> resourceLock(resourceMutex);
//     if(!resourcePtr)
//         resourcePtr.reset(new someResource);
//     resourceLock.unlock();
//     resourcePtr->doSomething();
// }

// // Double cbecked locking causes undefined behavior - nasty data race condition
// void doubleCheckedLocking() {
//     if(!resourcePtr) {
//         std::lock_guard<std::mutex> resourceLock(resourceMutex);
//         if(!resourcePtr)
//             resourcePtr.reset(new someResource);
//     }
//     resourcePtr->doSomething();
// }

void initResource() {
    resourcePtr.reset(new someResource);
}

void foo() {
    std::call_once(resourceFlag, initResource);
    resourcePtr->doSomething();
}

int main() {
    foo();

    return EXIT_SUCCESS;
}