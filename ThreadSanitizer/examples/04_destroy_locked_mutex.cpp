// TSAN example: destroying a locked mutex

#include <pthread.h>

int main(int argc, const char**)
{
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr);
    pthread_mutex_lock(&mutex);
    pthread_mutex_destroy(&mutex);

    return 0;
}
