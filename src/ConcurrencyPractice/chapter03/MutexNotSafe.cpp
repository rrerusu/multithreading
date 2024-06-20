#include <string>
#include <mutex>

class someData {
    private:
        int a;
        std::string b;
    public:
        void doSomething();
};

class dataWrapper {
    private:
        someData myData;
        std::mutex dataMutex;
    public:
        template<typename Function>
        void processData(Function func) {
            std::lock_guard<std::mutex> dataLockGuard(dataMutex);
            func(data);
        }
};

someData * unprotectedData;

void maliciousFunction(someData & protectedData) {
    unprotectedData = & protectedData;
}

dataWrapper myDataWrapper;

void foo() {
    myDataWrapper.processData(maliciousFunction);
    unprotectedData->doSomething();
}