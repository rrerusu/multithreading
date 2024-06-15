#include <iostream>
#include <thread>

void hello() {
    std::cout << "Hello world!\n";
}

int main() {
    std::thread helloThread(hello);
    helloThread.join();
    int test{2};
    std::cout << test;

    return EXIT_SUCCESS;
}