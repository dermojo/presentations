/**
 * @file    secret_application.cpp
 * @brief   a program that unveils its real functionality only to those who the secret passphrase...
 */

#include <fstream>
#include <iostream>

#include <cerrno>
#include <cstring>
#include <termios.h>
#include <unistd.h>

#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/wait.h>

#include <openssl/sha.h>

#include <boost/algorithm/string.hpp>

#include "syscalltable.hpp"

static bool debug = false;

static std::string queryPassphrase()
{
    std::cerr << "Password: ";

    // get current terminal attributes
    struct termios tty_attr;
    if (tcgetattr(STDIN_FILENO, &tty_attr) < 0)
        throw std::runtime_error("tcgetattr failed: " + std::string(strerror(errno)));

    // remember the current flags (to restore them later)
    const tcflag_t c_lflag = tty_attr.c_lflag;
    // disable echoing
    tty_attr.c_lflag &= ~ECHO;

    if (tcsetattr(STDIN_FILENO, 0, &tty_attr) < 0)
        throw std::runtime_error("tcsetattr failed: " + std::string(strerror(errno)));

    // now get the passphrase
    std::string input;
    std::getline(std::cin, input);
    std::cerr << "\n";

    // restore the terminal's old state
    tty_attr.c_lflag = c_lflag;
    if (tcsetattr(STDIN_FILENO, 0, &tty_attr) < 0)
        throw std::runtime_error("tcsetattr failed: " + std::string(strerror(errno)));

    return input;
}

static std::string sha256(const std::string& input)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    if (SHA256_Init(&sha256) != 1)
        throw std::runtime_error("SHA256_Init failed!");
    if (SHA256_Update(&sha256, input.data(), input.size()) != 1)
        throw std::runtime_error("SHA256_Update failed!");
    if (SHA256_Final(hash, &sha256) != 1)
        throw std::runtime_error("SHA256_Final failed!");

    // turn the binary data into a hex string (so we can print it)
    return std::string((const char*)hash, sizeof(hash));
}

#ifdef CHECK_FOR_DEBUGGER
/**
 * Check if a debugger is attached to this process.
 * @return @c true if debugged
 */
static bool isDebuggerAttached()
{
    // this only checks for a tracer *right now*, so this is actually a race condition...
    // -> use
    //         prctl(PR_SET_DUMPABLE, 0);
    //    to disable tracing first, then the check makes sense...

    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line))
    {
        if (boost::starts_with(line, "TracerPid:"))
        {
            line.erase(0, 10);
            boost::trim(line);
            // PID 0 is fine - no-one attached
            return line != "0";
        }
    }
    throw std::runtime_error("broken OS!");
}
#endif // CHECK_FOR_DEBUGGER

#ifdef PTRACE_MYSELF
static bool waitForSyscall(const pid_t pid)
{
    while (true)
    {
        ptrace(PTRACE_SYSCALL, pid, 0, 0);
        int status = 0;
        waitpid(pid, &status, 0);
        // check for 0x80 in the signal number (set due to PTRACE_O_TRACESYSGOOD)
        if (WIFSTOPPED(status) && (WSTOPSIG(status) & 0x80))
            return true;

        // process exited?
        if (WIFEXITED(status))
            return false;
    }
}

static void startPtrace()
{
    SyscallTable syscallTable;

    // to be more useful, parse syscall names from the installed headers
    if (debug)
        syscallTable.load("/usr/include/x86_64-linux-gnu/asm/unistd_64.h");

    // fork of a child process and trace it
    auto pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    if (pid == 0)
    {
        // child: allow the parent to start tracing here
        ptrace(PTRACE_TRACEME);
        // stop the current process, so that we wait for the parent to start tracing
        kill(getpid(), SIGSTOP);
        return;
    }

    // parent: wait for the child
    int status = 0;
    waitpid(pid, &status, 0);

    // configure tracing
    // (1) kill the child when this process exits
    ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL);
    // (2) when the child stops due to a syscall, deliver SIGTRAP|0x80 so that we know
    ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);

    if (debug)
        std::cout << "+++ Starting trace loop\n";
    while (true)
    {
        // wait for the child's next syscall or its termination
        if (!waitForSyscall(pid))
            break;

        // determine the syscall by reading the child's EAX (RAX for x86_64)
        // (the EAX register has been overwritten by the kernel for various reasons,
        //  but there is a backup copy we can use)
        long syscall = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * ORIG_RAX);
        const auto name = syscallTable.getSyscallName(syscall);

        if (!waitForSyscall(pid))
            break;

        // wait again for the syscall to return
        long retval = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * RAX);
        if (debug)
            std::cout << "+++ " << name << " returned with rc=" << retval << '\n';
    }

    if (debug)
        std::cout << "+++ Tracing done\n";
    exit(0);
}
#endif // PTRACE_MYSELF

/// SHA256 hash of the secret passphrase ("mellon")
static const char passphrase[] = "\x83\x00\x6a\x43\x8f\x94\xda\xf3\xa7\xdd\x9c\x7b\x27\xf7\x0c\x15"
                                 "\xe4\x43\xc0\xca\x55\xd5\x8f\xcd\xfa\x76\x89\x9a\xe4\x66\xb4\x55";

/**
 * Queries the user for a passphrase.
 * @return @c true if the passphrase is correct
 */
static bool checkAccess()
{
    // query the password first
    const std::string input = queryPassphrase();

    // hash the password
    const std::string hash = sha256(input);

    return hash == std::string(passphrase, sizeof(passphrase) - 1);
}

static void secretFunc()
{
    std::cout << "You got it!\n";
}


int main(int argc, const char** argv)
{
    for (int i = 1; i < argc; ++i)
        if (strcmp(argv[i], "--debug") == 0)
            debug = true;

#ifdef CHECK_FOR_DEBUGGER
    if (isDebuggerAttached())
    {
        std::cout << "detected attached debugging tool!\n";
    }
#endif // CHECK_FOR_DEBUGGER

#ifdef PTRACE_MYSELF
    startPtrace();
#endif // PTRACE_MYSELF

    if (checkAccess())
        secretFunc();
    else
        std::cerr << "Access denied!\n";
    return 0;
}
