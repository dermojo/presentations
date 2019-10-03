// UBSAN example: invalid bool
#include <cstring>

int main()
{
    bool b;
    memset(&b, 5, sizeof(b));
    return b ? 1 : 0;
}
