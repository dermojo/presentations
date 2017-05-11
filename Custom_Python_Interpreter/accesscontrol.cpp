/**
 * Access control function implementations.
 */

#include "accesscontrol.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

// you should hide this somehow...
static const char* const MAGIC_PASSWORD = "monkey123";

bool checkInteractiveAccess(bool isInteractive)
{
    // if running interactively, ask for the "magic password"
    if (isInteractive)
    {
        // note: never use getpass() in production code!
        const char* password = getpass("Speak friend and enter: ");
        if (password)
            return strcmp(password, MAGIC_PASSWORD) == 0;
    }
    return false;
}


SignatureStatus checkFileSignature(const wchar_t* file)
{
    // TODO: Python locale-specific conversion?
    char path[512];
    memset(path, 0, sizeof(path));
    wcstombs(path, file, sizeof(path) - 1);

    if (isStandalone(path))
        return checkStandaloneSignature(path);

    return checkDetachedSignature(path);
}
