#include <string>
#include <future>

struct X {
    void foo(int, std::string const &);
    std::string bar(std::string const &);
};

X x;
auto f1 = std::async(&X::foo, &x, 42, "Hello");
auto f2 = std::async(&X::bar, &x, "goodbye");



struct Y {
    double operator()(double);
};

Y y;
auto f3 = std::async(Y(), 3.141);
auto f4 = std::async(std::ref(y), 2.718);




X baz(X &);
auto f5 = std::async(baz, std::ref(x));

class MoveOnly {
    public:
        MoveOnly();
        MoveOnly(MoveOnly &&);
        MoveOnly(MoveOnly const &) = delete;
        MoveOnly & operator=(MoveOnly &&);
        MoveOnly & operator=(MoveOnly const &) = delete;

        void operator()();
};

auto f6 = std::async(MoveOnly());

auto f7 = std::async(std::launch::async, Y(), 1.2);
auto f8 = std::async(std::launch::deferred, baz, std::ref(x));
auto f9 = std::async(std::launch::async | std::launch::deferred, baz, std::ref(x));
auto f10 = std::async(baz, std::ref(x));

int main() {
    f8.wait();

    return 0;
}