#include <iostream>
#include <thread>

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
    // myThread.detach();

    // Good practice: always join threads even if there is error
    try {
        myThread.get_id();
        // Try to do something with thread
    } catch(...) {
        myThread.join();
        throw;
    }
    myThread.join();

    return EXIT_SUCCESS;
}