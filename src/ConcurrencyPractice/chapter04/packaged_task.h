#include <string>
#include <vector>
#include <future>

template<typename T>
class packaged_task {
    public:
        template<typename Callable>
        explicit packaged_task(Callable&& f);

        std::future<std::string> get_future();
        void operator()(T valueToPass);
};