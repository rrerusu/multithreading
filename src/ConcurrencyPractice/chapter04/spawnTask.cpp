#include <future>
#include <thread>
#include <type_traits>
#include <utility>
#include <iostream>

template<typename Func, typename Arg>
std::future<typename std::invoke_result<Func, Arg>::type> spawnTask(Func && f, Arg && a) {
    using resultType = typename std::invoke_result<Func, Arg>::type;

    std::packaged_task<resultType(Arg)> task(std::move(f));
    std::future<resultType> res = task.get_future();

    std::thread myThread([t = std::move(task), a = std::move(a)]() mutable { t(std::move(a)); });
    myThread.detach();

    return res;
}

int calcSquare(int num1) {
    return num1 * num1;
}

int main() {
    auto multiplyNums = spawnTask(&calcSquare, 5);

    std::cout << multiplyNums.get() << std::endl;

    return EXIT_SUCCESS;
}