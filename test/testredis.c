#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include "hiredis/hiredis.h"

int main()
{

    //redisContext *c = redisConnect("127.0.0.1", 6379);
    
    redisAsyncContext *c = redisAsyncConnect（ "127.0.0.1"，6379）;
    if（c->err）
    {
        printf（ "error\n"，c->errstr）;
        //处理错误
    }

    return 0;
}