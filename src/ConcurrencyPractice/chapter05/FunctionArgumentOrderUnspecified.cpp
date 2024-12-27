#include <iostream>

void foo(int a, int b) {
    std::cout << a << ", " << b << std::endl;
}

int getNum() {
    static int i = 0;
    return ++i;
}

int main() {
    foo(getNum(), getNum());

    return EXIT_SUCCESS;
}