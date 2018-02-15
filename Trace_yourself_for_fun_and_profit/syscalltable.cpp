/**
 * @file    syscalltable.cpp
 * @brief   helper functions for syscall infos
 */

#include "syscalltable.hpp"

#include <boost/lexical_cast.hpp>
#include <fstream>
#include <regex>

void SyscallTable::load(const char* path)
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
            m_syscallNames.insert(std::make_pair(number, name));
        }
    }
}

std::string SyscallTable::getSyscallName(long number) const
{
    auto entry = m_syscallNames.find(number);
    if (entry != m_syscallNames.end())
        return entry->second + "()";

    // unknown...
    return "'syscall " + std::to_string(number) + "'";
}
