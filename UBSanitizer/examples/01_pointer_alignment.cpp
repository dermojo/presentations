// UBSAN example: misaligned pointer access

int main(int, const char**)
{
    char buffer[123];
    int* ptr = (int*)(buffer + 1);
    *ptr = 42;
    return 0;
}
