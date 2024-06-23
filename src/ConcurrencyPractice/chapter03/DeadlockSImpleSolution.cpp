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
            // std::lock(lhs.accessMutex, rhs.accessMutex);
            // std::lock_guard<std::mutex> lockLHS(lhs.accessMutex, std::adopt_lock);
            // std::lock_guard<std::mutex> lockRHS(rhs.accessMutex, std::adopt_lock);
            std::unique_lock<std::mutex> lhs_lock(lhs.accessMutex, std::defer_lock);
            std::unique_lock<std::mutex> rhs_lock(rhs.accessMutex, std::defer_lock);
            std::lock(lhs_lock, rhs_lock);
            swap(lhs.someDetail, rhs.someDetail);
        }
};