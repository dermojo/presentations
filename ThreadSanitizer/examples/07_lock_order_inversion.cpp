// TSAN example: lock order inversion

#include <chrono>
#include <mutex>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>

using namespace std::chrono_literals;
using boost::system::error_code;

class Foo
{
public:
    Foo* m_other = nullptr;

    Foo(boost::asio::io_service& ctx) : m_timer(ctx) {}

    void startTimer()
    {
        m_timer.expires_from_now(10ms);
        m_timer.async_wait([this](const error_code& ec) {
            if (!ec)
            {
                onTimer();
            }
        });
    }

    void onTimer()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        ++m_calls;
        m_total = m_calls + m_other->getCalls();
    }

    size_t getCalls()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_calls;
    }

private:
    boost::asio::steady_timer m_timer;
    size_t m_calls = 0;
    size_t m_total = 0;
    std::mutex m_mutex;
};

int main()
{
    boost::asio::io_service ctx;

    Foo foo1(ctx);
    Foo foo2(ctx);
    foo1.m_other = &foo2;
    foo2.m_other = &foo1;

    foo1.startTimer();
    foo2.startTimer();

    ctx.run();

    return 0;
}
