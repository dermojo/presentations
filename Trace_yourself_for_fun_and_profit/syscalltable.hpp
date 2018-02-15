/**
 * @file    syscalltable.hpp
 * @brief   helper functions for syscall infos
 */

#ifndef TRACE_YOURSELF_FOR_FUN_AND_PROFIT_SYSCALLTABLE_HPP_
#define TRACE_YOURSELF_FOR_FUN_AND_PROFIT_SYSCALLTABLE_HPP_

#include <map>
#include <string>

/**
 * Table of all known syscalls, indexed by ID.
 */
class SyscallTable
{
public:
    /*
     * Adds syscall definitions from the given path.
     */
    void load(const char* path);
    /*
     * Look up a syscall name by it's number (returns a default string if unknown).
     */
    std::string getSyscallName(long number) const;

private:
    std::map<long, std::string> m_syscallNames;
};


#endif /* TRACE_YOURSELF_FOR_FUN_AND_PROFIT_SYSCALLTABLE_HPP_ */
