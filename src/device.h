#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include "hiredis/hiredis.h"
typedef unsigned short u16;
typedef unsigned char u8;
typedef struct
{
    u16 transaction; //标志
    u16 protocol;    //协议
    u16 len;         //长度
    u8 slave;        //单元标识符  应答复制
    u8 fun;          //函数
    u16 reg_str;     //寄存器开始地址
    u16 reg_num;     //寄存器数
    char *buffer;    //数据
    int bufferlen;
    int conn;
} modbus_request;

typedef struct
{
    u16 transaction; //标志
    u16 protocol;    //协议
    u16 len;         //长度
    u8 slave;        //单元标识符
    u8 fun;          //函数
    u8 bit_num;      //数据字节个数
    char *bit_data;  //数据
} modbus_response;

typedef struct Keys
{
    int size;
    char **keys;
    char *dev; //设备名
} Keys;
Keys keysAll; //全局存储key
typedef struct Devs
{
    int size;
    char **devs;
} Devs;
Devs devsAll;
typedef struct DeviceMemory
{
    char dev[5];
    float AI[99];
    float AO[99];
    float AV[99];
    char BI[99];
    char BO[99];
    char BV[99];
} DeviceMemory;
typedef struct DeviceMemoryAll
{
    DeviceMemory **dma;
    int size;
} DeviceMemoryAll;
typedef struct
{
    char key[8];
    char dev[5];
    char type[2];
    char number[3];
    int slave;
    int point;
} map_key;
typedef struct
{
    map_key **mks;
    int size;
} map_keys;
map_keys mks;

typedef struct
{
    char key[8];
    char dev[5];
    char type[2];
    char number[3];
    char pointType[2];
    int slave;
    int point;
    char value[20];
} xml_map_key;

typedef struct
{
    xml_map_key **xmks;
    int size;
} xml_map_keys;
xml_map_keys xmks;

DeviceMemoryAll dma;
DeviceMemory **DeviceMemorys; //静态的当前所有设备的内存
int DeviceMemorysCount;
redisContext *redis;
redisContext *keysredis;
redisContext *memoryredis;
int bit8ToInt(char *strbuf); //8位字符串转数字
char *intToChar(int num);    //数字转字符串
int Unique(char **devs, int len);
int getKeys(Keys *keys);
int getDevs(Devs *sdevs);
int initKeysAll();
int initDevsAll();
int getKeysForKeysAll(Keys *keys);
int getDeviceMemory(DeviceMemory *dm, char *dev);
void sortKeys(Keys *keys, int n);
int DeviceMemoryInit();
int DeviceMemoryAllUpdate();

void updateXmlMapKeys();
int freeKeys(Keys *keys);
int freeDevs(Devs *devs);
void catNumAdd0(char *buffer, int num);    //在字符串后面加上一个两位数的数字 如果该数字小于10则在前面补0
int bufAddSbit(char *resdata, char *sbit); //字符数组后面添加一个16进制数字，每8位字符串就是一个数字
static struct itimerval oldtv;

int redisInit();
int initDeviceMemoryAll();
int initMapKeys();

void set_timer();
void signal_handler(int m);

int getDevIndex(char *dev);
int getKeyIndex(char *key);
char *getKeyObjectName(char *key);
int redisSetValue(redisContext *redis, char *key, char *property, char *value);
char *redisGetValue(redisContext *redis, char *key, char *property);
int changePriority(redisContext *redis, char *key, char *value, int priority);
typedef struct updateModule
{
    int size;
    int slaves[100];
} updateModule;
updateModule uM;
int initUpdateModule();
int updateModuleAddSlave(int slave);
int updateModuleIsHaveSlave(int salve);

//unsigned long jsqCount;
//int updateKeysAll();

xml_map_key* findXMKByXmlMapKey(int slave, int point,char pointType);

extern int print;