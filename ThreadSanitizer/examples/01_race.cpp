// TSAN example: race condition

#include <iostream>
#include <thread>

static size_t counter = 0;

static void run()
{
    for (int i = 0; i < 10000; ++i)
    {
        ++counter;
    }
}

int main()
{
    std::thread t1([] { run(); });
    std::thread t2([] { run(); });

    t1.join();
    t2.join();

    std::cout << "result: " << counter << "\n";

    return 0;
}
