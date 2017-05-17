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
#include "hiredis/hiredis.h"
#include "asynreids.h"

#ifndef include_device //X为你的标识符,保持唯一，可以长点，比如 #ifndef _INCLUDE_XXXXXX_H_
#define include_device
#include "device.h"
#endif

//#include "mxml-release-2.10/mxml.h"
#define MYPORT 8888
#define QUEUE 20
#define BUFFER_SIZE 1024
#define SOCK_PORT 8888
#define BUFFER_LENGTH 1024
#define MAX_CONN_LIMIT 512 //MAX connection limit

int fun01(modbus_request *mrq, char *resdata);
int fun02(modbus_request *mrq, char *resdata);
int fun03(modbus_request *mrq, char *resdata);
int fun04(modbus_request *mrq, char *resdata);

int readMessage(char *buffer, int len, int conn);
//static const char *netnum;

void run();

int socket_run();
static void Data_handle(void *sock_fd);



void *asynRedis(void *arg);
int runThread();