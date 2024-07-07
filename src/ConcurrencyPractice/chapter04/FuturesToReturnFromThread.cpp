#include <future>
#include <iostream>

int findTheAnswerToLTUAE();
void doOtherStuff();

int main() {
    std::future<int> theAnswer = std::async(findTheAnswerToLTUAE);
    doOtherStuff();
    std::cout << "The answer is " << theAnswer.get() << std::endl;

    return EXIT_SUCCESS;
}