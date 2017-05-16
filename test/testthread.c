#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "hiredis/hiredis.h"
#include "hiredis/async.h"
#include "hiredis/adapters/libevent.h"
static int count = 0;
void *thread_function(void *arg);
void *thread_function(void *arg)
{
    while (1)
    {
        printf("asdasda");

        sleep(0);
    }
    pthread_exit(NULL);
}
int redisSubscribeRun()
{
    int res;
    pthread_t a_thread;
    void *thread_result;
    res = pthread_create(&a_thread, NULL, thread_function, NULL);
    res = pthread_join(a_thread, &thread_result);
    return 0;
}
int main()
{
    redisSubscribeRun();
    
    return 0;
}
