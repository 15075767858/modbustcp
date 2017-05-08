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
#include "hiredis/hiredis.h"
#define MYPORT 8888
#define QUEUE 20
#define BUFFER_SIZE 1024

typedef unsigned short u16;
typedef unsigned char u8;
//1001001
//网络号 模块地址 点位类型 点位编号
//单元标识符对应网络号 模块地址对应模块地址 函数对应的应该是功能代码 寄存器开始地址 应该是点位编号
//06 设备地址 寄存器地址 数值

typedef struct
{
    u16 transaction; //标志
    u16 protocol;    //协议
    u16 len;         //长度
    u8 slave;        //单元标识符  应答复制
    u8 fun;          //函数
    u16 reg_str;     //寄存器开始地址
    u16 reg_num;     //寄存器数
} modbus_request;
typedef struct
{
    u16 transaction; //标志
    u16 protocol;    //协议
    u16 len;         //长度
    u8 slave;        //单元标识符
    u8 fun;          //函数
    u8 bit_num;      //字节个数
    char *bit_data;  //数据
} modbus_response;

modbus_request readMessage(char *buffer, int len);
void catNumAdd0(char *buffer, int num);

//在字符串后面加上一个两位数的数字
//如果该数字小于10则在前面补0
void catNumAdd0(char *buffer, int num)
{
    char buf[10];
    memset(buf, 0, 10);
    if (num < 10)
    {
        sprintf(buf, "%s%d", "0", num);
    }
    else
    {
        sprintf(buf, "%d", num);
    }
    strcat(buffer, buf);
}
modbus_response readAI(modbus_request *mrq)
{
    modbus_response mrqp;
    return mrqp;
}

modbus_request readMessage(char *buffer, int len)
{

    modbus_request mrq;
    mrq.transaction = buffer[0] * 256 + (u8)buffer[1];
    mrq.protocol = buffer[2] * 256 + buffer[3];
    mrq.len = buffer[4] * 256 + buffer[5];
    mrq.slave = buffer[6];                       //网络号
    mrq.fun = (int)buffer[7];                    //点位类型
    mrq.reg_str = buffer[8] * 256 + buffer[9];   //点位编号
    mrq.reg_num = buffer[10] * 256 + buffer[11]; //数量

    // modbus_request mrp;
    // mrp.transaction = mrq.transaction;
    // mrp.protocol = mrq.protocol;
    // mrp.len = mrq.len; //待修改
    // mrp.slave = mrq.slave;

    if (len - 6 != mrq.len)
    {
        printf("数据长度错误 %d %d \n", len - 6, mrq.len);
    }
    char key[10];
    memset(key, 0, 10);
    char *netnum = "1100";
    key[0] = netnum[0];
    key[1] = netnum[1];
    catNumAdd0(key, mrq.slave);
    char buf[10];
    memset(buf, 0, 10);
    sprintf(buf, "%hhu", mrq.fun);
    strcat(key, buf);
    //catNumAdd0(key, mrq.reg_str);

    redisContext *redis = redisConnect("127.0.0.1", 6379);
    if (mrq.fun == 1)
    {
        for (int i = 0; i < mrq.reg_num; i++)
        {
            char skey[10];
            memset(skey, 0, 10);
            strcat(skey, key);
            catNumAdd0(skey, i + mrq.reg_str);
            printf("skey = %s \n", skey);
            printf();
　　　　　　　
            char commandbuffer[100];
            sprintf(commandbuffer, "hget %s Present_Value", skey);
            redisReply *rest = (redisReply *)redisCommand(redis, commandbuffer);
            //printf(rest->str);
            //printf("第 %d 寄存器 \n", i + mrq.reg_str);
        }
    }
    redisFree(redis);

    printf("len=%x %x %x\n", mrq.transaction, mrq.protocol, mrq.len);
    return mrq;
}

void test()
{
    ///定义sockfd
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ///定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(MYPORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    ///bind，成功返回0，出错返回-1
    if (bind(server_sockfd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    ///listen，成功返回0，出错返回-1
    if (listen(server_sockfd, QUEUE) == -1)
    {
        perror("listen");
        exit(1);
    }
    printf("start listen\n");
    ///客户端套接字
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    while (1)
    {
        int conn = accept(server_sockfd, (struct sockaddr *)&client_addr, &length);
        ///成功返回非负描述字，出错返回-1
        printf("start accept\n");
        if (conn < 0)
        {
            perror("connect");
            exit(1);
        }
        while (1)
        {
            memset(buffer, 0, sizeof(buffer));
            int len = recv(conn, buffer, sizeof(buffer), 0);
            if (len == 0)
            {
                break;
            }
            readMessage(buffer, len);

            //fputs(buffer, stdout);
            send(conn, "111", len, 0);
        }
    }
    printf("close\n");
    //close(conn);
    close(server_sockfd);
}

void redisTest()
{
}

int main()
{
    //redisTest();
    test();
    return 0;
}
