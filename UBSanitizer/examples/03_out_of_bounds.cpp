// UBSAN example: array out-of-bounds

int main()
{
    int array[5];
    for (int i = 0; i <= 5; ++i)
        array[i] = 0;
    return 0;
}

