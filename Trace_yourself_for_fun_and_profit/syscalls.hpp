/**
 * @file    syscalls.hpp
 * @brief   helper functions for syscall infos
 */

#ifndef TRACE_YOURSELF_FOR_FUN_AND_PROFIT_SYSCALLS_HPP_
#define TRACE_YOURSELF_FOR_FUN_AND_PROFIT_SYSCALLS_HPP_

#include <fstream>
#include <map>
#include <regex>
#include <string>

#include <boost/lexical_cast.hpp>

static std::map<long, std::string> s_syscallNames;

static void readSyscallNames(const char* path)
{
    // syscalls are defined as: #define __NR_<NAME> <NUMBER>
    const std::regex pattern("^#define\\s*__NR_(\\w+)\\s*(\\d+)");
    std::smatch match;

    std::ifstream ifs(path);
    std::string line;
    while (std::getline(ifs, line))
    {
        if (std::regex_match(line, match, pattern))
        {
            auto number = boost::lexical_cast<long>(match[2]);
            const auto& name = match[1];
            s_syscallNames.insert(std::make_pair(number, name));
        }
    }
}

static std::string getSyscallName(long number)
{
    auto entry = s_syscallNames.find(number);
    if (entry != s_syscallNames.end())
        return entry->second + "()";

    // unknown...
    return "'syscall " + std::to_string(number) + "'";
}


#endif /* TRACE_YOURSELF_FOR_FUN_AND_PROFIT_SYSCALLS_HPP_ */
