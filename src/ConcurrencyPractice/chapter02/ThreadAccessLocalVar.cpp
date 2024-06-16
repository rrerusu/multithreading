#include <iostream>
#include <thread>

class threadGuard {
    std::thread & myThread;
    public:
        explicit threadGuard(std::thread & myThread_) : myThread(myThread_) {}
        ~threadGuard() {
            if(myThread.joinable())
                myThread.join();
        }
        threadGuard(threadGuard const &) = delete;
        threadGuard & operator=(threadGuard const &) = delete;
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
    func myFunc(someLocalState);
    std::thread myThread(myFunc);
    threadGuard myThreadGuard(myThread);

    // do something in thread
    myThread.get_id();

    return EXIT_SUCCESS;
}