#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#define NUM 2
static int  count = 0;
void *thread_function(void *arg);

int main()
{
    int res;
    pthread_t a_thread[NUM];
    void *thread_result;
    int index;
    char *aaa = "asd";
    for (index = 0; index < NUM; ++index)
    {

        res = pthread_create(&a_thread[index], NULL, thread_function, (void *)aaa);
        if (res != 0)
        {
            perror("Thread create failed!");
            exit(EXIT_FAILURE);
        }
        // sleep(1);
    }

    printf("Waiting for threads to finished.../n");

    for (index = NUM - 1; index >= 0; --index)
    {
        res = pthread_join(a_thread[index], &thread_result);
        if (res == 0)
        {
            printf("Picked up a thread:%d/n", index + 1);
        }
        else
        {
            perror("pthread_join failed/n");
        }
    }

    printf("All done/n");

    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg)
{
    int rand_num;
    while (1)
    {
        printf("Argument was %d /n ",count++);
        //sleep(1);
    }

    //rand_num = 1 + (int)(9.0 * rand()/(RAND_MAX + 1.0));
    //sleep(rand_num);
    //printf("Bye from %d/n", my_number);
    pthread_exit(NULL);
}