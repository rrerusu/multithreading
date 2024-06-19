#include <iostream>
#include <thread>

// class threadGuard {
//     std::thread & myThread;
//     public:
//         explicit threadGuard(std::thread & myThread_) : myThread(myThread_) {}
//         ~threadGuard() {
//             if(myThread.joinable())
//                 myThread.join();
//         }
//         threadGuard(threadGuard const &) = delete;
//         threadGuard & operator=(threadGuard const &) = delete;
// };
class ScopedThread {
    std::thread myThread;
    public:
        explicit ScopedThread(std::thread myThread_) : myThread(std::move(myThread_)){
            if(!myThread.joinable())
                throw std::logic_error("No thread");
        }
        ~ScopedThread() {
            myThread.join();
        }
        ScopedThread(ScopedThread const &) = delete;
        ScopedThread & operator=(ScopedThread const &) = delete;
};

struct func {
    int & i;
    func(int & _i) : i(_i) {}
    void operator() () {
        for(unsigned j = 0; j < 1000; j++) {
            std::cout << j << std::endl;
        }
    }
    void test() {
        std::cout << "test" << std::endl;
    }
};

int main() {
    int someLocalState = 0;
    // func myFunc(someLocalState);
    ScopedThread myScopedThread(std::thread(func(someLocalState)));

    // do something in thread

    return EXIT_SUCCESS;
}