#include <mutex>

class Y {
    private:
        int someDetail;
        mutable std::mutex classMutex;
        int getDetail() const {
            std::lock_guard<std::mutex> classLock(classMutex);
            return someDetail;
        }
    public:
        Y(int sd) : someDetail(sd) {}
        friend bool operator==(Y const & lhs, Y const & rhs) {
            if(&lhs == &rhs)
                return true;
            int const lhsValue = lhs.getDetail();
            int const rhsValue = rhs.getDetail();
            return lhsValue == rhsValue;
        }
};