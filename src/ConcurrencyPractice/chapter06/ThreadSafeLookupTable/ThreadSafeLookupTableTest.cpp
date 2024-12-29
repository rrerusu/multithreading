#include <iostream>

#include "ThreadSafeLookupTable.h"

int main() {
    ThreadSafeLookupTable<int, int, std::hash<int>> myTable;

    myTable.addOrUpdateMapping(1, 1);
    myTable.addOrUpdateMapping(2, 2);
    myTable.addOrUpdateMapping(3, 3);
    myTable.addOrUpdateMapping(4, 4);
    myTable.addOrUpdateMapping(4, 5);
    myTable.removeMapping(4);

    std::map<int, int> tableAsMap = myTable.getMap();

    for(auto mapIt = tableAsMap.begin(); mapIt != tableAsMap.end(); ++mapIt)
        std::cout << mapIt->first << " " << mapIt->second << std::endl;

    return 0;
}