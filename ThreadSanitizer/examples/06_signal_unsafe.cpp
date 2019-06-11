// TSAN example: signal handler unsafe malloc/free

#include <csignal>
#include <string>

#include <sys/types.h>
#include <unistd.h>

static void handler(int, siginfo_t*, void*)
{
    std::string x(128, 'x');
}

int main()
{
    struct sigaction act = {};
    act.sa_sigaction = handler;
    sigaction(SIGINT, &act, 0);

    kill(getpid(), SIGINT);

    return 0;
}
