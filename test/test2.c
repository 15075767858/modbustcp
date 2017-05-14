#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include "hiredis/hiredis.h"

typedef struct Keys
{
    int size;
    char **keys;
    char *dev; //设备名
} Keys;
typedef struct Devs
{
    int size;
    char **devs;
} deviceSizeStrs;
typedef struct DeviceMemory
{
    char *dev;
    float AI[512];
    float AO[512];
    float AV[512];
    char BI[512];
    char BO[512];
    char BV[512];
} DeviceMemory;
static redisContext *redis;

static DeviceMemory **DeviceMemorys; //静态的当前所有设备的内存
static int DeviceMemorysCount = 0;
static Keys globalKeys;
static deviceSizeStrs globalDevs;
int redisInit();
int Unique(char **devs, int len);
int getKeys(Keys *keys);
int getDevs(deviceSizeStrs *sdevs);
int getDeviceMemory(DeviceMemory *dm, char *dev);
void sortKeys(Keys *keys, int n);
int DeviceMemoryInit();
int Unique(char **devs, int len)
{

    int i, count = 0;
    char *res[len];

    for (i = 0; i < len; i++)
    {
        char *dev = devs[i];
        int j, ishave = 0;
        for (j = 0; j < count; j++)
        {
            if (strncmp(dev, res[j], 4) == 0) //相等说明存在
            {
                ishave = 1;
                break;
            }
        }
        if (ishave == 0)
        {
            res[count] = devs[i];
            devs[count] = devs[i];
            count++;
        }
    }
    return count;
}
int getDevs(deviceSizeStrs *sdevs)
{

    Keys keys;
    keys.dev = NULL;
    getKeys(&keys);

    char *devs[keys.size];
    int i, count = 0;
    for (i = 0; i < keys.size; i++)
    {
        // char *str = keys.keys[i];// char *devnum = (char *)malloc(5);// strncat(devnum, str, 4);// devs[i] = devnum;
        keys.keys[i][4] = '\0';
        devs[i] = keys.keys[i];
    }

    count = Unique(devs, keys.size);

    sdevs->size = count;

    sdevs->devs = (char **)calloc(count, sizeof(char *));
    for (i = 0; i < count; i++)
    {
        //sdevs->devs[i] = strdup(devs[i]);
        sdevs->devs[i] = devs[i];
        printf("dev%d=(%s) ", i, sdevs->devs[i]);
    }
    free(keys.keys);
    return 0;
}
int getKeys(Keys *keys)
{

    //redisContext *c = redisConnect("127.0.0.1", 6379);

    char *command = (char *)malloc(100);
    //strcat(command, "keys ");
    command[0] = 'k';
    command[1] = 'e';
    command[2] = 'y';
    command[3] = 's';
    command[4] = ' ';
    command[5] = '\0';

    if (keys->dev != NULL)
    {
        strcat(command, keys->dev);
        printf("\n dev= %s \n", keys->dev);
    }
    else
    {
        strncat(command, "???????\0", 8);
        printf("\n 没有dev  \n");
    }
    printf("command == ( %s )\n", command);
    int y;
    for (y = 0; y < 15; y++)
    {
        printf(" %c ", command[y]);
    }
    printf("\n");

    redisReply *reply = (redisReply *)redisCommand(redis, command);
    memset(command, 0, 100);
    free(command);

    int k;
    int len = reply->elements;
    printf("key(");
    for (k = 0; k < len; k++)
    {
        printf("%s ", reply->element[k]->str);
    }
    printf(")\n");
    if (len == 0)
    {
        keys->size = len;
        return 0;
    }

    keys->keys = (char **)calloc(reply->elements, sizeof(char *));

    int i;
    int count = 0;
    printf("\n len = (%d)  \n", len);

    for (i = 0; i < len; i++)
    {
        if (reply->element[i]->str[4] < '6')
        {
            keys->keys[count] = reply->element[i]->str;
            count++;
        }
    }
    printf("jieshu\n");
    keys->size = count;
    sortKeys(keys, keys->size - 1);
    freeReplyObject(reply);
    // for (i = 0; i < keys->size; i++)
    // {
    //     printf("key%d=(%s) ", i, keys->keys[i]);
    // }
    //redisFree(c);
    return 0;
}
int getDeviceMemory(DeviceMemory *dm, char *dev)
{
    //redisContext *c = redisConnect("127.0.0.1", 6379);

    int i, types = 6;
    for (i = 0; i < types; i++)
    {
        char command[10];
        memset(command, 0, 10);
        sprintf(command, "%s%d%s", dev, i, "??\0");
        Keys keys;
        keys.dev = command;

        getKeys(&keys);
        memset(command, 0, 10);
        //free(command);

        printf("findkey = (%s) size = (%d)\n", command, keys.size);

        int j;
        for (j = 0; j < keys.size; j++)
        {
            char *command1 = (char *)malloc(30);
            sprintf(command1, "%s %s %s", "hget", keys.keys[j], "Present_Value");
            redisReply *reply = (redisReply *)redisCommand(redis, command1);
            printf("command1 = (%s) value = (%s)", command1, reply->str);
            free(command1);

            printf("isnull= (%d) ", reply->str == 0);
            if (reply->str == 0)
            {
                reply->str = "0\0";
            }
            switch (i)
            {
            case 0:
                dm->AI[j] = atof(reply->str);
                break;
            case 1:
                dm->AO[j] = atof(reply->str);
                break;
            case 2:
                dm->AV[j] = atof(reply->str);
                break;
            case 3:
                dm->BI[j] = atoi(reply->str);
                break;
            case 4:
                dm->BO[j] = atoi(reply->str);
                break;
            case 5:
                dm->BV[j] = atoi(reply->str);
                break;
            }
        }

        printf("end   \n");
        // int j;
        // for (j = 0; j < reply->elements; j++)
        // {
        //     printf("--value =  %s --\n", reply->element[j]->str);
        // }
    }
    //redisFree(c);

    return 0;
}
void sortKeys(Keys *keys, int n)
{

    int i, j;
    char *k;
    for (j = 0; j < n; j++) /* 气泡法要排序n次*/
    {
        for (i = 0; i < n - j; i++) /* 值比较大的元素沉下去后，只把剩下的元素中的最大值再沉下去就可以啦 */
        {
            if (atoi(keys->keys[i]) > atoi(keys->keys[i + 1])) /* 把值比较大的元素沉到底 */
            {
                k = keys->keys[i];
                keys->keys[i] = keys->keys[i + 1];
                keys->keys[i + 1] = k;
            }
        }
    }
}
static struct itimerval oldtv;
void set_timer()
{
    struct itimerval itv;
    itv.it_interval.tv_sec = 1;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itv, &oldtv);
}
void signal_handler(int m)
{
    DeviceMemoryInit();
    printf("%s\n", "timer runing");
}

int redisInit()
{
    redisContext *c = redisConnect("127.0.0.1", 6379);
    redis = c;
    return 0;
}

int init()
{
    redisInit();
    signal(SIGALRM, signal_handler);
    set_timer();
    while (1)
        ;
    return 0;
    //DeviceMemoryInit();
}
int main()
{
    redisInit();
    //init();
    Keys keys;
    keys.dev = NULL;

    while (1)
 
        //getKeys(&keys);
    return 0;
}

int DeviceMemoryInit()
{
    deviceSizeStrs devs;
    getDevs(&devs);
    //free(devs.devs);
    //return 0;
    //DeviceMemory **dms = (DeviceMemory **)calloc(devs.size, sizeof(DeviceMemory));
    DeviceMemory *dms[sizeof(DeviceMemory) * devs.size];
    int i;
    for (i = 0; i < devs.size; i++)
    {
        DeviceMemory *a = (DeviceMemory *)malloc(sizeof(DeviceMemory));
        a->dev = devs.devs[i];
        getDeviceMemory(a, devs.devs[i]);
        dms[i] = a;
    }
    DeviceMemorys = dms;
    DeviceMemorysCount = i;
    free(devs.devs);
    return 0;
}