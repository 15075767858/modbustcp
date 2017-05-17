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
#include <hiredis.h>
#include <signal.h>
#include <adapters/libevent.h>

//#include <adapters/libevent.h>
//设置命令执行后的回调函数
//gcc  ./output/async.o ./output/dict.o ./output/net.o ./output/read.o ./output/sds.o ./output/hiredis.o   test/testredis.c  -o a.out  -g  -I./hiredis/ -L/usr/include/sys
void *thread_function(void *arg);
void asyn();

int getDevIndex(char *dev)
{
    int index = 0;
    char **devs; //={"1001","1002","1003","1004","1005"};
    devs[0] = "1001";
    devs[1] = "1002";
    devs[2] = "1003";
    devs[3] = "1004";
    return index;
}

void *thread_function(void *arg)
{
    printf("thread run");
    sleep(0);
    asyn();
    pthread_exit(NULL);
}
int redisSubscribeRun()
{
    int res;
    pthread_t a_thread;
    void *thread_result;
    res = pthread_create(&a_thread, NULL, thread_function, NULL);
    pthread_t b_thread;
    void *thread_result1;
    res = pthread_create(&b_thread, NULL, thread_function, NULL);
    res = pthread_join(a_thread, &thread_result);
    res = pthread_join(b_thread, &thread_result1);
    return 0;
}

void getCallback(redisAsyncContext *c, void *r, void *privdata)
{
    redisReply *reply = r;
    if (reply->elements == 4)
    {
        char *str = strdup(reply->element[3]->str);
        //printf("\n %s\n", str);
        char key[50];
        char type[100];
        char val[10240];
        sscanf(str, "%s\r\n%s\r\n%s", key, type, val);

        printf("%s %s %s \n", key, type, val);
        free(str);
    }

    //freeReplyObject(reply);
    if (reply == NULL)
        return;
    //printf("argv[%s]: %s\n", (char *)privdata, reply->str);
}

void connectCallback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK)
    {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Connected...\n");
}

void disconnectCallback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK)
    {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Disconnected...\n");
}

void asyn()
{
    signal(SIGPIPE, SIG_IGN);
    struct event_base *base = event_base_new();
    redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 6379);
    if (c->err)
    {
        printf("Error:redis %s\n", c->errstr);
    }
    redisLibeventAttach(c, base);
    redisAsyncSetConnectCallback(c, connectCallback);
    redisAsyncSetDisconnectCallback(c, disconnectCallback);
    redisAsyncCommand(c, getCallback, (char *)"sub", "PSUBSCRIBE *");
    event_base_dispatch(base);
    //环境  回调函数 私人传值 命令 参数。。。
}

int main(int argc, char **argv)
{
    getDevIndex("1002");
    redisSubscribeRun();
    return 0;
}
