// TSAN example: thread leak

#include <pthread.h>

void* threadFunc(void*)
{
    return 0;
}

int main()
{
    // note: only possible in C ;-)
    pthread_t t;
    pthread_create(&t, nullptr, threadFunc, nullptr);
    // pthread_join(t, nullptr);

    return 0;
}
