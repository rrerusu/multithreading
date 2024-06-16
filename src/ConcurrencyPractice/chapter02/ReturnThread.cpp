#include <thread>
#include <utility>

std::thread f() {
    void someFunction();
    return std::thread(someFunction);
}
std::thread g() {
    void someOtherFunction(int);
    std::thread t(someOtherFunction, 42);
    return t;
}

void h(std::thread t);

void i() {
    void someThirdFunction();
    h(std::thread(someThirdFunction));
    std::thread t(someThirdFunction);
    h(std::move(t));
}