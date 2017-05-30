#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include "hiredis/hiredis.h"
#include <signal.h>
#include <pthread.h>
#include <adapters/libevent.h>

#ifndef include_device //X为你的标识符,保持唯一，可以长点，比如 #ifndef _INCLUDE_XXXXXX_H_
#define include_device
#include "device.h"
#endif


void asyn();
int changeDeviceMemory(char *key, char *value);
void disconnectCallback(const redisAsyncContext *c, int status);
void connectCallback(const redisAsyncContext *c, int status);
void getCallback(redisAsyncContext *c, void *r, void *privdata);


extern int print;