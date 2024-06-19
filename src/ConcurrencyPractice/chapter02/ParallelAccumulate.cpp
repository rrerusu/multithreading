#include <iostream>
#include <thread>
#include <algorithm>
#include <functional>
#include <numeric>

template<typename Iterator, typename T>
struct accumulateBlock {
    void operator()(Iterator first, Iterator last, T & result) {
        result = std::accumulate(first, last, result);
    }
};

template<typename Iterator, typename T>
T parallelAccumulate(Iterator first, Iterator last, T init) {
    unsigned long const LENGTH = std::distance(first, last);

    if(!LENGTH)
        return init;
    
    unsigned long const MIN_PER_THREAD = 25;
    unsigned long const MAX_THREADS = (LENGTH + MIN_PER_THREAD - 1) / MIN_PER_THREAD;

    unsigned long const HARDWARE_THREADS = std::thread::hardware_concurrency();

    unsigned long const NUM_THREADS = std::min(HARDWARE_THREADS != 0 ? HARDWARE_THREADS : 2, MAX_THREADS);

    unsigned long const BLOCK_SIZE = LENGTH / NUM_THREADS;

    std::vector<T> results(NUM_THREADS);
    std::vector<std::thread> simpleThreadPool(NUM_THREADS - 1);

    Iterator blockStart = first;
    for(unsigned long i = 0; i < NUM_THREADS - 1; i++) {
        Iterator blockEnd = blockStart;
        std::advance(blockEnd, BLOCK_SIZE);
        simpleThreadPool[i] = std::thread(
            accumulateBlock<Iterator, T>(),
            blockStart, blockEnd, std::ref(results[i])
        );
        blockStart = blockEnd;
    }
    accumulateBlock<Iterator, T>()(blockStart, last, results[NUM_THREADS - 1]);
    std::for_each(simpleThreadPool.begin(), simpleThreadPool.end(),
        std::mem_fn(&std::thread::join));
    
    return std::accumulate(results.begin(), results.end(), init);
}

int main() {
    std::vector<int> numbers;
    numbers.push_back(1);
    numbers.push_back(2);
    numbers.push_back(3);
    numbers.push_back(4);
    numbers.push_back(5);

    int result = parallelAccumulate<std::vector<int>::iterator, int>(numbers.begin(), numbers.end(), 0);
    std::cout << "Result: " << result << std::endl;

    return EXIT_SUCCESS;
}