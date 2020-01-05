#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#include <boost/algorithm/string.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/process.hpp>

namespace bp = boost::process;

using namespace std::literals::chrono_literals;

#if 0
static void printDiff(const std::vector<std::string>& s1, const std::vector<std::string>& s2)
{
    const size_t maxLines = std::max(s1.size(), s2.size());
    const std::string empty;
    for (size_t i = 0; i < maxLines; ++i)
    {
        const std::string& left = (i < s1.size() ? s1[i] : empty);
        const std::string& right = (i < s2.size() ? s2[i] : empty);
        char ch = (left == right ? '=' : '!');

        std::cout <<"{} | {: <75.75} | {: <75.75}\n", ch, left, right);
    }
}

static void printDiff(const std::string& s1, const std::string& s2)
{
    std::vector<std::string> lines1, lines2;
    boost::split(lines1, s1, boost::is_any_of("\n"), boost::token_compress_off);
    boost::split(lines2, s2, boost::is_any_of("\n"), boost::token_compress_off);
    printDiff(lines1, lines2);
}
#endif

static std::string readFile(const std::string& path)
{
    std::ifstream ifs(path);
    std::string content, line;
    while (std::getline(ifs, line))
    {
        content += line;
        content += '\n';
    }

    if (ifs.bad())
        throw std::runtime_error("failed to read " + path);

    return content;
}

#ifdef WIN32
static const std::string file = "../sample.cpp";
// copied from: C:\Program Files (x86)\Microsoft Visual
// Studio\2017\Community\Common7\IDE\CommonExtensions\Microsoft\TeamFoundation\Team
// Explorer\Git\usr\bin\cat.exe
static const std::string program = "./cat.exe";
#else
static const std::string file = "/proc/mounts";
static const std::string program = "/bin/cat";
#endif

static void cat()
{
    const std::string refContent = readFile(file);

    bp::ipstream out;
    bp::child proc(program, file, bp::std_out > out);
    std::string content, line;
    while ( // proc.running() &&
      std::getline(out, line))
    {
        content += line;
        content += '\n';
    }

    proc.wait();
    const int rc = proc.exit_code();
    if (rc != 0)
        std::cerr << "exit code: " << rc << '\n';
    if (content != refContent)
    {
        std::cerr << "content mismatch!\n";
        // printDiff(content, refContent);
    }
}

static void cat_async()
{
    const std::string refContent = readFile(file);

    boost::asio::io_service ctx;

    bp::async_pipe out(ctx);
    bp::child proc(program, file, bp::std_out > out, ctx);
    std::string content;

    char buffer[1024];
    std::function<void()> asyncRead;
    auto readCb = [&](const boost::system::error_code& ec, std::size_t size) {
        if (!ec)
        {
            content.append(buffer, size);
            asyncRead();
        }
        else
        {
            std::cout << "read error: " << ec.message() << " (" << ec << ")\n";
        }
    };
    asyncRead = [&] { out.async_read_some(boost::asio::buffer(buffer), readCb); };
    asyncRead();

    ctx.run();

    proc.wait();
    const int rc = proc.exit_code();
    if (rc != 0)
        std::cerr << "exit code: " << rc << '\n';
    if (content != refContent)
    {
        std::cerr << "content mismatch!\n";
        // printDiff(content, refContent);
    }
}

static void fail()
{
    try
    {
        bp::system("no_such_proc");
    }
    catch (const bp::process_error& err)
    {
        std::cerr << "Exception (expected): " << err.what() << '\n';
    }
}

int main()
{
    try
    {
        std::cout << "======= Running cat =======\n";
        cat();
        std::cout << "======= Running cat_async =======\n";
        cat_async();
#ifdef WIN32
        std::cout << "======= Running notepad =======\n";
        bp::system("notepad");
#else
        std::cout << "======= Running ls =======\n";
        bp::system("ls -l");
#endif
        std::cout << "======= Running fail =======\n";
        fail();

        std::cout << "======= Env check =======\n";
        auto env = boost::this_process::environment();
        env["MYPATH"] = {"bar1", "bar2"};
        std::cout << "MYPATH=" << env.at("MYPATH").to_string() << "\n";

        return 0;
    }
    catch (const std::exception& exc)
    {
        std::cerr << "unhandled exception in main(): " << exc.what() << "\n";
        return 1;
    }
}
