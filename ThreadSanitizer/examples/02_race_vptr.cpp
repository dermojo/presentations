// TSAN example: vptr race

#include <atomic>
#include <iostream>
#include <thread>

class A
{
public:
    virtual ~A()
    {
        while (!m_done)
            std::this_thread::yield();
    }
    virtual void foo() = 0;

    void done() { m_done = true; }

    std::atomic<bool> m_done{ false };
};

class B : public A
{
public:
    virtual ~B() = default;
    void foo() override {}
};


int main()
{
    std::unique_ptr<A> b(new B());

    std::thread t1([&] {
        // use the vptr by calling a virtual function
        b->foo();
        b->done();
    });
    std::thread t2([&] {
        // modify the vptr by destroying the object
        b.reset();
    });

    t1.join();
    t2.join();

    return 0;
}
