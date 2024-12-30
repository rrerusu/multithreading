#include <iostream>

#include "ThreadSafeList.h"

int main() {
    ThreadSafeList<int> myList;

    myList.push_front(1);

    myList.for_each([](int & val){ val++; });
    // Can pass as copy, but if it is a large object this would be slow
    std::cout << *myList.find_first_if([](int val){ return val == 2; }) << std::endl;

    myList.push_front(1);
    myList.for_each([](int & val){ val++; });
    myList.for_each([](int & val){ val++; });
    myList.for_each([](int & val){ std::cout << val << " "; });
    std::cout << std::endl;

    myList.remove_if([](int & val){ return val == 3; });
    myList.for_each([](int & val){ std::cout << val << " "; });
    std::cout << std::endl;

    myList.insert(10, 1);
    myList.for_each([](int & val){ std::cout << val << " "; });
    std::cout << std::endl;

    myList.insert(-1, 50);

    return EXIT_SUCCESS;
}