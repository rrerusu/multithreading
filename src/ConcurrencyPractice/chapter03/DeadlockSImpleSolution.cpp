#include <mutex>

class someBigObject {
    public:
        someBigObject(const someBigObject &);
};

void swap(someBigObject & lhs, someBigObject & rhs);

class X {
    private:
        someBigObject someDetail;
        std::mutex accessMutex;
    public:
        X(someBigObject const & sd) : someDetail(sd) {}
        friend void swap(X & lhs, X & rhs) {
            if(& lhs == & rhs)
                return;
            std::lock(lhs.accessMutex, rhs.accessMutex);
            std::lock_guard<std::mutex> lockLHS(lhs.accessMutex, std::adopt_lock);
            std::lock_guard<std::mutex> lockRHS(rhs.accessMutex, std::adopt_lock);
            swap(lhs.someDetail, rhs.someDetail);
        }
};