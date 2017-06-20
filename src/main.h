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
#include <pthread.h>
#include <assert.h>
#include <sys/utsname.h>
#include "hiredis/hiredis.h"
#include "asynreids.h"
#include "devicexml.h"
#ifndef include_device //X为你的标识符,保持唯一，可以长点，比如 #ifndef _INCLUDE_XXXXXX_H_
#define include_device
#include "device.h"
#endif

//#include "mxml-release-2.10/mxml.h"
// #define SOCK_PORT 502
// #if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) || defined(Q_OS_WINCE)
// #define SOCK_PORT 8888
// #endif
// #if defined(Q_OS_DARWIN64)
#define SOCK_PORT 8889
//#define SOCK_PORT 502

#define BUFFER_LENGTH 102400

#define MAX_CONN_LIMIT 512 //MAX connection limit
int socketCount;
int fun01(modbus_request *mrq, char *resdata);
int fun02(modbus_request *mrq, char *resdata);
int fun03(modbus_request *mrq, char *resdata);
int fun04(modbus_request *mrq, char *resdata);
int fun05(modbus_request *mrq, char *resdata);
int fun06(modbus_request *mrq, char *resdata);
int fun15(modbus_request *mrq, char *resdata);
int fun16(modbus_request *mrq, char *resdata);
int readMessage(char *buffer, int len, int conn);
int saveMessage(char *buffer, int len);
//static const char *netnum;

int socket_run();
static void Data_handle(void *sock_fd);
void *socketStart(void *arg);
void *asynRedis(void *arg);
void *DeviceMemoryAllUpdateStart(void *arg);
int runThread();

int isMac();

char *getDevBySlave(int slave);
char getTypeByFun(int fun);
//char *getPointNumberByPoint(char *dev, char type, int point);
char *getKeyBySlavePoint(modbus_request *mrq);
void error_quit();
int print =1;
void savePid();
