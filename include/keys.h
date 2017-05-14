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
    float AI[512];
    float AO[512];
    float AV[512];
    char BI[512];
    char BO[512];
    char BV[512];
} DeviceMemory;
//static DeviceMemory **DeviceMemorys; //静态的当前所有设备的内存
int Unique(char **devs, int len);
int getKeys(Keys *keys);
int getDevs(deviceSizeStrs *sdevs);
int getDeviceMemory(DeviceMemory *dm, char *dev);
void sortKeys(Keys *keys, int n);
int DeviceMemoryInit();