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
    u8 bit_num;      //数据字节个数
    char *bit_data;  //数据
} modbus_response;
char *resBuffer;
modbus_request readMessage(char *buffer, int len, int conn);
void catNumAdd0(char *buffer, int num);    //在字符串后面加上一个两位数的数字 如果该数字小于10则在前面补0
int bit8ToInt(char *strbuf);               //8位字符串转数字
char *intToChar(int num);                  //数字转字符串
int bufAddSbit(char *resdata, char *sbit); //字符数组后面添加一个16进制数字，每8位字符串就是一个数字

modbus_response readAI(modbus_request *mrq)
{
    modbus_response mrqp;
    return mrqp;
}

modbus_request readMessage(char *buffer, int len, int conn)
{
    modbus_request mrq;
    mrq.transaction = buffer[0] * 256 + (u8)buffer[1];
    mrq.protocol = buffer[2] * 256 + buffer[3];
    mrq.len = buffer[4] * 256 + buffer[5];
    mrq.slave = buffer[6];                       //设备地址
    mrq.fun = (int)buffer[7];                    //点位类型
    mrq.reg_str = buffer[8] * 256 + buffer[9];   //点位编号
    mrq.reg_num = buffer[10] * 256 + buffer[11]; //数量

    char resdata[100];
    memset(resdata, 0, 100);
    //strncpy(resdata, buffer, 8);
    resdata[0] = buffer[0];
    resdata[1] = buffer[1];
    resdata[2] = buffer[2];
    resdata[3] = buffer[3];
    resdata[4] = 0;         //高位
    resdata[5] = buffer[5]; //低位
    resdata[6] = buffer[6];
    resdata[7] = buffer[7];
    resdata[8] = 0x0; //字节数量
    modbus_request mrp;
    mrp.transaction = mrq.transaction;
    mrp.protocol = mrq.protocol;
    //mrp.len = mrq.len; //待修改
    mrp.slave = mrq.slave;
    mrp.fun = mrq.fun;

    if (len - 6 != mrq.len)
    {
        printf("data length error %d %d \n", len - 6, mrq.len);
    }
    char key[10];
    memset(key, 0, 10);
    char *netnum = "1000";
    key[0] = netnum[0];
    key[1] = netnum[1];
    catNumAdd0(key, mrq.slave);

    redisContext *redis = redisConnect("127.0.0.1", 6379);

    if (mrq.fun == 1) //AO 1
    {
        key[4] = '0';
        int i;
        char sbit[100];
        memset(sbit, 0, 100);
        for (i = 0; i < mrq.reg_num; i++)
        {
            char skey[10];
            memset(skey, 0, 10);
            strcat(skey, key);
            catNumAdd0(skey, i + mrq.reg_str + 1);
            char commandbuffer[100];
            sprintf(commandbuffer, "hget %s Present_Value", skey);
            redisReply *rest = (redisReply *)redisCommand(redis, commandbuffer);
            printf("key== %s ", skey);
            if (rest > 0 && rest->str > 0)
            {
                sbit[i] = '1';
            }
            else
            {
                sbit[i] = '0';
            }
        }
        int dtlen = bufAddSbit(resdata, sbit);
        resdata[5] = dtlen + 3;
        send(conn, resdata, 9 + dtlen, 0);
    }
    if (mrq.fun == 6) //02 ab 00 00 00 06 ff 06 00 02 00 03
    {
        key[4] = '4';
        char skey[10];
        memset(skey, 0, 10);
        strcat(skey, key);
        catNumAdd0(skey, i + mrq.reg_str + 1);
        int addr = buffer[8] * 256 + buffer[9];
        int data = buffer[10] * 256 + buffer[11];
        char commandbuffer[100];
        sprintf(commandbuffer, "hget %s Present_Value", skey);
        redisReply *rest = (redisReply *)redisCommand(redis, commandbuffer);
    }
    if (mrq.fun == 3) //BI 3
    {

        key[4] = '4';
        int i;
        char sbit[100];
        memset(sbit, 0, 100);
        resdata[8] = 0;
        int count = 0;
        for (i = 0; i < mrq.reg_num; i++)
        {
            char skey[10];
            memset(skey, 0, 10);
            strcat(skey, key);
            catNumAdd0(skey, i + mrq.reg_str + 1);
            char commandbuffer[100];
            sprintf(commandbuffer, "hget %s Present_Value", skey);
            redisReply *rest = (redisReply *)redisCommand(redis, commandbuffer);
            printf("key== %s %s\n", skey, rest->str);
            if (rest > 0 && rest->str > 0)
            {
                int val = atoi(rest->str);
                resdata[9 + count] = val >> 8;
                resdata[10 + count] = val & 0x00FF;
                //printf("val= %d res= %d %d\n", val, resdata[9 + i], resdata[10 + i]);
            }
            else
            {
                resdata[9 + count] = 0;
                resdata[10 + count] = 0;
            }
            count += 2;
            resdata[8] = count;
        }
        send(conn, resdata, 9 + resdata[8], 0);
    }
    if (mrq.fun == 3) //
    {
    }
    redisFree(redis);
    //printf("len=%x %x %x\n", mrq.transaction, mrq.protocol, mrq.len);
    return mrq;
}

int bufAddSbit(char *resdata, char *sbit)
{
    //修改返回值为int 表示增加的字节数
    //while 逻辑不通顺
    char strbuf[10];
    memset(strbuf, 0, 10);
    int i, reg_data = 0;
    int len = strlen(sbit);
    int count = 1; //返回值长度

    for (i = 0; i < len; i += 8)
    {
        strncpy(strbuf, sbit, 8);
        sbit = sbit + 8;
        reg_data = bit8ToInt(strbuf);
        resdata[8] = count;
        resdata[9 + count - 1] = reg_data;
        printf("9+i = %d\n", 9 + count - 1);
        resdata[10 + i] = '\0';
        printf("bb=%x %d %d\n", resdata[9 + count - 1], reg_data, count);
        reg_data = 0;
        count++;
    }
    return count - 1;
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
            printf("data = %s \n", buffer);
            int i;
            for (i = 0; i < 20; i++)
            {
                printf(" %02d ", buffer[i]);
            }

            char resdata[100];
            memset(resdata, 0, 100);
            readMessage(buffer, len, conn);

            //fputs(buffer, stdout);

            //send(conn, resdata, len, 0);
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
int bit8ToInt(char *strbuf)
{
    int i, reg_data = 0;
    int len = strlen(strbuf);
    for (i = 0; i < len; i++)
    {
        reg_data = reg_data << 1;
        if (strbuf[i] == '1')
        {
            reg_data = reg_data + 1;
        }
    }
    return reg_data;
}
char *intToChar(int num)
{
    char numbuf[100];
    memset(numbuf, 0, 100);
    sprintf(numbuf, "%d", num);
    char *aa = numbuf;
    return aa;
}

void catNumAdd0(char *buffer, int num)
{
    char buf[10];
    memset(buf, 0, 10);
    sprintf(buf, "%02d", num);
    strcat(buffer, buf);
}