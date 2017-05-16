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
//#include "mxml-release-2.10/mxml.h"
#define MYPORT 8888
#define QUEUE 20
#define BUFFER_SIZE 1024
typedef unsigned short u16;
typedef unsigned char u8;
static const char *netnum;
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
typedef struct Devs
{
    int size;
    char **devs;
} Devs;
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
typedef struct DeviceMemoryAll
{
    DeviceMemory **dma;
    int size;
} DeviceMemoryAll;
DeviceMemoryAll dma;
DeviceMemory **DeviceMemorys; //静态的当前所有设备的内存
int DeviceMemorysCount = 0;
int Unique(char **devs, int len);
int getKeys(Keys *keys);
int getDevs(Devs *sdevs);
int getDeviceMemory(DeviceMemory *dm, char *dev);
void sortKeys(Keys *keys, int n);
int DeviceMemoryInit();
int redisInit();
int DeviceMemoryAllUpdate();
int freeKeys(Keys *keys);
int freeDevs(Devs *devs);
int fun01(modbus_request *mrq, char *resdata);
int fun02(modbus_request *mrq, char *resdata);
int fun03(modbus_request *mrq, char *resdata);
int fun04(modbus_request *mrq, char *resdata);

int initDeviceMemoryAll();
int readMessage(char *buffer, int len, int conn);
void catNumAdd0(char *buffer, int num);    //在字符串后面加上一个两位数的数字 如果该数字小于10则在前面补0
int bit8ToInt(char *strbuf);               //8位字符串转数字
char *intToChar(int num);                  //数字转字符串
int bufAddSbit(char *resdata, char *sbit); //字符数组后面添加一个16进制数字，每8位字符串就是一个数字