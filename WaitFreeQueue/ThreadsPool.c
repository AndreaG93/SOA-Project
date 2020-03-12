#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include "ThreadsPool.h"

pthread_t *startThreadPool(unsigned long long poolSize, void *(*thread_routine)(void *), void *threadInputData) {

    pthread_t *output = malloc(poolSize * sizeof(pthread_t));
    if (output == NULL)
        exit(EXIT_FAILURE);
    else
        for (unsigned long long index = 0; index < poolSize; index++) {

            if (pthread_create(&output[index], NULL, thread_routine, threadInputData) != 0)
                exit(EXIT_FAILURE);
        }

    return output;
}

void joinAndFreeThreadsPool(pthread_t *threadPoll, unsigned long long poolSize) {

    for (unsigned long long index = 0; index < poolSize; index++) {
        pthread_join(threadPoll[index], NULL);

    }
    free(threadPoll);
}