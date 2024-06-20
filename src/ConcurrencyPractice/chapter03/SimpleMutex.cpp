#include <list>
#include <mutex>
#include <algorithm>
#include <thread>
#include <iostream>
#include <vector>

std::list<int> myList;
std::mutex listMutex;

void addToList(int val) {
    std::lock_guard<std::mutex> listGuard(listMutex);
    myList.push_back(val);
}
bool listContains(int findMe) {
    std::lock_guard<std::mutex> guard(listMutex);
    return std::find(myList.begin(), myList.end(), findMe) != myList.end();
}
void checkIfContaine(int findMe) {
    std::cout << "Is " << findMe << " in the list? " << (listContains(findMe) ? "yes" : "no") << std::endl;
}

int main() {
    std::vector<std::thread> simpleThreadPool;
    for(int i = 1; i <= 5; i++)
        simpleThreadPool.push_back(std::thread(addToList, i));
    
    for(int i = 1; i <= 2; i++)
        simpleThreadPool.push_back(std::thread(checkIfContaine, 4 + i));

    for(int i = 0; i < simpleThreadPool.size(); i++)
        simpleThreadPool[i].join();

    return EXIT_SUCCESS;
}