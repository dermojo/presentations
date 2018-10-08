// ASAN example: global buffer overflow

#include <cstdio>

static char greeting[64];

int main(int argc, char** argv)
{
    const char* name = argv[1];
    sprintf(greeting, "Hello, %s!\n", name);
    puts(greeting);
    return 0;
}
