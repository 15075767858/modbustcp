#include "asynreids.h"

//修改内存页的值
int changeDeviceMemory(char *key, char *value)
{
    char dev[10];
    memset(dev, 0, 10);
    strncat(dev, key, 4);
    int devIndex = getDevIndex(dev);
    int keyIndex = getKeyIndex(key);
    if (devIndex < 0 || keyIndex < 0)
    {
        return -1;
    }
    if (strncmp(dma.dma[devIndex]->dev, dev, 4) != 0)
    {
        printf("dev not cmp %s", dev);
    }
    //printf("dev = %s\n", dma.dma[devIndex]->dev);
    switch (key[4])
    {
    case '0': //AI
        dam.AI[devIndex * 100 + keyIndex] = atof(value);
        break;
    case '1': //AO
        dam.AO[devIndex * 100 + keyIndex] = atof(value);
        break;
    case '2': //AV
        dam.AV[devIndex * 100 + keyIndex] = atof(value);
        break;
    case '3': //BI
        dam.BI[devIndex * 100 + keyIndex] = atoi(value);
        break;
    case '4': //BO
        dam.BO[devIndex * 100 + keyIndex] = atoi(value);
        break;
    case '5': //BV
        dam.BV[devIndex * 100 + keyIndex] = atoi(value);
        break;
    default: //other
        break;
    }
    return 0;
}

//订阅回调函数
void getCallback(redisAsyncContext *c, void *r, void *privdata)
{
    redisReply *reply = r;
    if (reply->elements == 4)
    {
        char key[50];
        char type[100];
        char val[1024];
        sscanf(reply->element[3]->str, "%s\r\n%s\r\n%s", key, type, val);
        if (strncmp(type, "Present_Value", 13) == 0)
        {
            changeDeviceMemory(key, val);
        }
        if (print == 0)
            printf("pubdata =  %s %s %s ", key, type, val);
    }
    if (reply == NULL)
        return;
}
//异步订阅
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

//int main(int argc, char **argv)
//{
//    redisInit();
//    initDeviceMemoryAll();
//    signal(14, signal_handler);
//    set_timer();
//    printf("%d\n", getDevIndex("1001"));
//    printf("keyindex =  (%d)\n", getKeyIndex("1001108"));
//    redisSubscribeRun();
//    return 0;
//}
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
