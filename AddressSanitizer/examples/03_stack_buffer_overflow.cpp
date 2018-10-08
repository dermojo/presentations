// ASAN example: stack buffer overflow

#pragma GCC diagnostic ignored "-Warray-bounds"

int main(int argc, char** argv)
{
    int stack_array[100];
    stack_array[1] = 0;
    return stack_array[100 + argc]; // BOOM
}
