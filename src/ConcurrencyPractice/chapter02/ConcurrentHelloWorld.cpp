#include <iostream>
#include <thread>

void hello() {
    std::cout << "Hello world!\n";
}

int main() {
    std::thread helloThread(hello);
    helloThread.join();

    return EXIT_SUCCESS;
}