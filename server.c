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
#include "mxml-release-2.10/mxml.h"
#define MYPORT 8888
#define QUEUE 20
#define BUFFER_SIZE 1024
typedef unsigned short u16;
typedef unsigned char u8;
static const char *netnum;
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
static DeviceMemory **DeviceMemorys; //静态的当前所有设备的内存
static int DeviceMemorysCount = 0;
int Unique(char **devs, int len);
int getKeys(Keys *keys);
int getDevs(deviceSizeStrs *sdevs);
int getDeviceMemory(DeviceMemory *dm, char *dev);
void sortKeys(Keys *keys, int n);
int DeviceMemoryInit();
int redisInit();

int fun01(modbus_request *mrq, char *resdata);
int fun02(modbus_request *mrq, char *resdata);
int fun03(modbus_request *mrq, char *resdata);
int fun04(modbus_request *mrq, char *resdata);

int readMessage(char *buffer, int len, int conn);
void catNumAdd0(char *buffer, int num);    //在字符串后面加上一个两位数的数字 如果该数字小于10则在前面补0
int bit8ToInt(char *strbuf);               //8位字符串转数字
char *intToChar(int num);                  //数字转字符串
int bufAddSbit(char *resdata, char *sbit); //字符数组后面添加一个16进制数字，每8位字符串就是一个数字
int initNetNum();
static redisContext *redis;

static struct itimerval oldtv;

int resolveMessageToModbusRequest(modbus_request *mrq)
{

    //功能吗01对应04 02对应03  03对应02  04对应0
    int ms_len, ms_fun, ms_slavedev, ms_reg_begin, ms_reg_size;

    return 0;
}
int fun01(modbus_request *mrq, char *resdata) //BO
{
    //mrq->reg_str = mrq->reg_str + 1;
    printf("fun03  slave=(%d) reg_str=(%d) reg_num=(%d)\n", mrq->slave, mrq->reg_str, mrq->reg_num);
    DeviceMemory dm = DeviceMemorys[mrq->slave - 1][0];
    int start = mrq->reg_str;
    int end = mrq->reg_num;
    int count = 0;
    int i;
    char str[mrq->reg_num + 1];
    int jishu = 0;
    memset(str, 0, mrq->reg_num);
    for (i = start; i < end + start; i++)
    {
        int val;
        if (mrq->fun == 1)
        {
            val = dm.BO[i];
        }
        else
        {
            val = dm.BI[i];
        }
        if (val != 0)
        {
            str[count] = '1';
        }
        else
        {
            str[count] = '0';
        }
        str[count + 1] = '\0';
        printf("count = %d str=%s i = %d end =  %d yushu = %d strlen =%lu \n", count, str, i, end, end % 8, strlen(str));
        count++;
    }

    char *a = &str[0];
    char sb[8]; //一次装8个数字
    memset(sb, 0, 8);
    int s8 = strlen(a) / 8 + 1; //循环次数

    for (i = 0; i < s8; i++)
    {
        strncpy(sb, a, 8);
        printf("bit8ToInt(sb) = %d\n", bit8ToInt(sb));
        a += 8;
        int res = bit8ToInt(sb);
        resdata[9 + i] = res;
    }
    // int slen = strlen(str);
    //bit8ToInt(strbuf)
    //bufAddSbit(resdata, str);
    printf("\n str = (%s) ", str);
    send(mrq->conn, resdata, 100, 0);
    return 0;
    //1010001000
    // 0100010000
}
int fun02(modbus_request *mrq, char *resdata) //BI
{
    fun01(mrq, resdata);
    return 0;
}

int fun04(modbus_request *mrq, char *resdata)
{
    fun03(mrq, resdata);
    return 0;
}
int fun03(modbus_request *mrq, char *resdata) //AV
{
    printf("fun03  slave=(%d) reg_str=(%d) reg_num=(%d)\n", mrq->slave, mrq->reg_str, mrq->reg_num);
    DeviceMemory dm = DeviceMemorys[mrq->slave - 1][0];
    int start = mrq->reg_str;
    int end = mrq->reg_num;
    int count = 0;
    int i;
    for (i = start; i < end + start; i++)
    {
        union {
            uint8_t byte[8];
            float real_value;
        } my_data;
        if (mrq->fun == 3)
        {
            printf(" i=(%d) value = (%f) \n", i, dm.AV[i]);
            my_data.real_value = dm.AV[i];
        }
        else
        {
            printf(" i=(%d) value = (%f) \n", i, dm.AI[i]);
            my_data.real_value = dm.AI[i];
        }
        resdata[9 + count * 4] = my_data.byte[1];
        resdata[10 + count * 4] = my_data.byte[0];
        resdata[11 + count * 4] = my_data.byte[3];
        resdata[12 + count * 4] = my_data.byte[2];
        count++;
    }
    printf("resdata=(");
    for (i = 0; i < 8 + resdata[8] + 1; i++)
    {
        printf("%02hhx ", resdata[i]);
    }
    printf(");\n");
    send(mrq->conn, resdata, 8 + resdata[8] + 1, 0);
    return 0;
}
int readMessage(char *buffer, int len, int conn)
{

    int reg_num = buffer[10] * 256 + buffer[11] - 1;
    modbus_request mrq;
    mrq.transaction = buffer[0] * 256 + (u8)buffer[1];
    mrq.protocol = buffer[2] * 256 + buffer[3];
    mrq.len = buffer[4] * 256 + buffer[5];
    mrq.slave = buffer[6];                     //设备地址
    mrq.fun = (int)buffer[7];                  //功能码
    mrq.reg_str = buffer[8] * 256 + buffer[9]; //点位编号
    mrq.reg_num = reg_num;                     //数量
    mrq.buffer = buffer;
    mrq.bufferlen = len;
    mrq.conn = conn;

    char resdata[100];
    memset(resdata, 0, 100);
    //strncpy(resdata, buffer, 8);
    resdata[0] = buffer[0];
    resdata[1] = buffer[1];
    resdata[2] = buffer[2];
    resdata[3] = buffer[3];
    resdata[4] = 0; //高位
    resdata[5] = 7; //低位
    resdata[6] = buffer[6];
    resdata[7] = buffer[7];

    if (buffer[7] == 2 || buffer[7] == 1)
    {
        if (reg_num % 8 == 0)
        {
            resdata[8] = reg_num / 8;
        }
        else
        {
            resdata[8] = reg_num / 8 + 1;
        }
        //resdata[7] = ;
    }
    if (buffer[7] == 4 || buffer[7] == 3)
    {
        resdata[8] = reg_num * 4; //字节数量
    }
    int len1 = 3 + resdata[8];

    resdata[4] = len1 >> 8;
    resdata[5] = len1 & 0x00FF;
    if (len - 6 != mrq.len)
    {
        printf("data length error %d %d \n", len - 6, mrq.len);
        send(conn, buffer, len, 0);
        return 1;
    }
    switch (mrq.fun)
    {
    case 1:
        fun01(&mrq, resdata);
        break;
    case 2:
        fun02(&mrq, resdata);
        break;
    case 3:
        fun03(&mrq, resdata);
        break;
    case 4:
        fun04(&mrq, resdata);
        break;
    case 6:
        break;
    default:
        send(conn, resdata, 8, 0);

        break;
    }
    //     key[4] = '4';
    //     int addr = buffer[8] * 256 + buffer[9];
    //     int data = buffer[10] * 256 + buffer[11];
    //     char skey[10];
    //     memset(skey, 0, 10);
    //     //sprintf(skey,)
    //     strcat(skey, key);
    //     catNumAdd0(skey, addr + 1);
    //     printf("key = %s\n", skey);
    //     char commandbuffer[100];
    //     sprintf(commandbuffer, "hset %s Present_Value %d", skey, data);
    //     redisReply *rest = (redisReply *)redisCommand(redis, commandbuffer);

    //     send(conn, buffer, 12, 0);
    return 0;
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
    for (i = 0; i < len / 8 + 1; i++)
    {
        strncpy(strbuf, sbit, 8);
        reg_data = bit8ToInt(strbuf);
        resdata[9 + i] = reg_data;
    }
    return 0;
    // for (i = 0; i < len; i += 8)
    // {
    //     strncpy(strbuf, sbit, 8);
    //     sbit = sbit + 8;
    //     reg_data = bit8ToInt(strbuf);
    //     resdata[8] = count;
    //     resdata[9 + count - 1] = reg_data;
    //     printf("9+i = %d\n", 9 + count - 1);
    //     resdata[10 + i] = '\0';
    //     printf("bb=%x %d %d\n", resdata[9 + count - 1], reg_data, count);
    //     reg_data = 0;
    //     count++;
    // }
    // return count - 1;
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
            int i;
            printf("data =(");
            for (i = 0; i < len; i++)
            {
                printf("%02hhx ", buffer[i]);
            }
            printf(" )\n");
            readMessage(buffer, len, conn);
        }
    }
    printf("close\n");
    //close(conn);
    close(server_sockfd);
}


int bit8ToInt(char *strbuf)
{
    char resbuf[9];
    memset(resbuf,0,9);
    resbuf[0]=strbuf[7];
    resbuf[1]=strbuf[6];
    resbuf[2]=strbuf[5];
    resbuf[3]=strbuf[4];
    resbuf[4]=strbuf[3];
    resbuf[5]=strbuf[2];
    resbuf[6]=strbuf[1];
    resbuf[7]=strbuf[0];
    resbuf[8]='\0';
    int i, reg_data = 0;
    int len = strlen(strbuf);
    for (i = 0; i < 8; i++)
    {
        reg_data = reg_data << 1;
        if (resbuf[i] == '1')
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

int initNetNum()
{
    FILE *fp;
    mxml_node_t *tree;

    fp = fopen("./bac_config.xml", "r");
    if (fp == 0)
    {
        printf("/mnt/nandflash/bac_config.xml not found\n");
        return 1;
    }
    tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    mxml_node_t *node;
    node = mxmlFindElement(tree, tree, "net", NULL, NULL, MXML_DESCEND);

    netnum = mxmlGetText(node, 0);
    printf("net number = %s \n", netnum);
    fclose(fp);
    return 0;
}

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
        strcat(command, "???????\0");
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

        //char *key =reply->element[i]->str;// strdup(reply->element[i]->str);
        if (reply->element[i]->str[4] < '6')
        {
            keys->keys[count] = reply->element[i]->str;
            count++;
        }
    }

    printf("jieshu\n");

    keys->size = count;

    sortKeys(keys, keys->size - 1);
    // for (i = 0; i < keys->size; i++)
    // {
    //     printf("key%d=(%s) ", i, keys->keys[i]);
    // }
    //redisFree(c);
    //freeReplyObject(reply);
    return 0;
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
            freeReplyObject(reply);
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

int DeviceMemoryInit()
{
    deviceSizeStrs devs;
    getDevs(&devs);
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
    printf(" %s ", " devs.devs[i]");
    DeviceMemorys = dms;
    DeviceMemorysCount = i;
    free(devs.devs);
    return 0;
}
void signal_handler(int m)
{
    // if (DeviceMemorysCount != 0)
    // {
    //     int i;
    //     for (i = 0; i < DeviceMemorysCount; i++)
    //     {
    //         free(DeviceMemorys[i]);
    //     }
    //     free(DeviceMemorys);
    // }
    //free(DeviceMemorys[0]);
    // free(DeviceMemorys[1]);
    
    DeviceMemoryInit();
    printf("%s\n", "timer runing");
}
void set_timer()
{
    struct itimerval itv;
    itv.it_interval.tv_sec = 10;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itv, &oldtv);
}

int redisInit()
{
    redisContext *c = redisConnect("127.0.0.1", 6379);
    //err 1 errstr[128]
    if (c->err != 0)
    {
        printf("redis %s \n", c->errstr);
        exit(0);
        return c->err;
    }
    redis = c;
    return 0;
}
int main()
{
    redisInit();
    signal(14, signal_handler);
    set_timer();
    //DeviceMemoryInit();
    sleep(10);
    printf("run server\n");
    test();
    //initNetNum();
    //redisTest();
    //10100010000
    redisFree(redis);
    return 0;
}