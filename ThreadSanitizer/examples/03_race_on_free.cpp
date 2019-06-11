// TSAN example: race on free

#include <atomic>
#include <iostream>
#include <thread>

class A
{
public:
    ~A()
    {
        while (!m_done)
            std::this_thread::yield();
    }
    void foo() {}

    void done() { m_done = true; }

    std::atomic<bool> m_done{ false };
};


int main()
{
    std::unique_ptr<A> a(new A());

    std::thread t1([&] {
        // use A
        a->foo();
        a->done();
    });
    std::thread t2([&] {
        // destroy A
        a.reset();
    });

    t1.join();
    t2.join();

    return 0;
}
