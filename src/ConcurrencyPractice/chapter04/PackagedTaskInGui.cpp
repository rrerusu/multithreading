#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <utility>

std::mutex taskMutex;
std::deque<std::packaged_task<void()>> tasks;
bool guiShutdownMessageReceived();
void getAndProcessGuiMessage();

void gui_thread() {
    while(!guiShutdownMessageReceived())
        getAndProcessGuiMessage();
        std::packaged_task<void()> task;
        {
            std::lock_guard<std::mutex> taskMutexLock(taskMutex);
            // if(tasks.empty())
            //     continue;
            task = std::move(tasks.front());
            tasks.pop_front();
        }
        task();
}

std::thread gui_bg_thread(gui_thread);

template<typename Func>
std::future<void> postTaskForGuiThread(Func myFunc) {
    std::packaged_task<void()> task(myFunc);
    std::future<void> res = task.get_future();
    tasks.push_back(std::move(task));
    return res;
}