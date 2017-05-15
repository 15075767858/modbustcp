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
#include <async.h>
#include <signal.h>
#include <adapters/libevent.h>
//#include <adapters/libevent.h>
//设置命令执行后的回调函数
//gcc  ./output/async.o ./output/dict.o ./output/net.o ./output/read.o ./output/sds.o ./output/hiredis.o   test/testredis.c  -o a.out  -g  -I./hiredis/ -L/usr/include/sys
void getCallback(redisAsyncContext *c, void *r, void *privdata)
{
    redisReply *reply = r;
    if (reply == NULL)
        return;
    printf("argv[%s]: %s\n", (char *)privdata, reply->str);

    /* Disconnect after receiving the reply to GET */
    redisAsyncDisconnect(c);
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

int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);
    printf("start");

    struct event_base *base = event_base_new();
    redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 6379);
    if (c->err)
    {
        /* Let *c leak for now... */
        printf("Error: %s\n", c->errstr);
        return 1;
    }
    redisLibeventAttach(c, base);
    redisAsyncSetConnectCallback(c, connectCallback);
    redisAsyncSetDisconnectCallback(c, disconnectCallback);
    redisAsyncCommand(c, NULL, NULL, "SET key %b", argv[argc - 1], strlen(argv[argc - 1]));
    redisAsyncCommand(c, getCallback, (char *)"end-1", "GET key");
    event_base_dispatch(base);
    return 0;
}
