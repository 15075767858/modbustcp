#include "main.h"

int main()
{
    //初始化redis
    redisInit();
    //初始化全局所有key，方便后面用不再从数据库查询
    initKeysAll();
    initDevsAll();
    initUpdateModule();
    printf("redis run \n");
    //初始化设备内存页
    initDeviceMemoryAll();
    //初始化设备内存
    initDeviceXml();
    //开启10秒一次更新内存页
    signal(14, signal_handler);
    set_timer();
    //开启多线程访问
    runThread();
    printf("asyn redis run\n");
    return 0;
}
//是否是mac
int isMac()
{
    struct utsname un;
    uname(&un);

    return strncmp(un.sysname, "Darwin", 5);
}
int socket_run()
{
    int sockfd_server;
    int sockfd;
    int fd_temp;
    struct sockaddr_in s_addr_in;
    struct sockaddr_in s_addr_client;
    int client_length;
    socketCount = 0;
    sockfd_server = socket(AF_INET, SOCK_STREAM, 0); //ipv4,TCP
    assert(sockfd_server != -1);
    memset(&s_addr_in, 0, sizeof(s_addr_in));
    s_addr_in.sin_family = AF_INET;
    s_addr_in.sin_addr.s_addr = htonl(INADDR_ANY); //trans addr from uint32_t host byte order to network byte order.
    int port = 502;
    if (isMac() == 0)
    {
        port = 8888;
    }
    s_addr_in.sin_port = htons(port); //trans port from uint16_t host byte order to network byte order.
    fd_temp = bind(sockfd_server, (struct scokaddr *)(&s_addr_in), sizeof(s_addr_in));

    if (fd_temp == -1)
    {
        fprintf(stderr, "bind error!\n");
        exit(1);
    }

    fd_temp = listen(sockfd_server, MAX_CONN_LIMIT);
    if (fd_temp == -1)
    {
        fprintf(stderr, "listen error!\n");
        exit(1);
    }

    while (1)
    {
        // if (socketCount >= 10)
        // {
        //     fprintf(stderr, "pthread_create error! max is %d\n", 10);
        //     continue;
        // }
        printf("waiting for new connection...\n");
        pthread_t thread_id;
        client_length = sizeof(s_addr_client);

        //Block here. Until server accpets a new connection.
        sockfd = accept(sockfd_server, (struct sockaddr_ *)(&s_addr_client), (socklen_t *)(&client_length));

        if (sockfd == -1)
        {
            fprintf(stderr, "Accept error!\n");
            continue; //ignore current socket ,continue while loop.
        }
        socketCount++;

        printf("A new connection occurs!\n");
        if (pthread_create(&thread_id, NULL, (void *)(&Data_handle), (void *)(&sockfd)) == -1)
        {
            fprintf(stderr, "pthread_create error!\n");
            break; //break while loop
        }
    }
    //Clear
    int ret = shutdown(sockfd_server, SHUT_WR); //shut down the all or part of a full-duplex connection.
    assert(ret != -1);
    printf("Server shuts down\n");
    return 0;
}

static void Data_handle(void *sock_fd)
{

    int fd = *((int *)sock_fd);
    int i_recvBytes;
    char data_recv[BUFFER_LENGTH];
    int errcount = 0;

    while (1)
    {
        memset(data_recv, 0, BUFFER_LENGTH);
        i_recvBytes = read(fd, data_recv, BUFFER_LENGTH);
        int i;
        printf("reqdata=(");
        for (i = 0; i < i_recvBytes; i++)
        {
            printf("%02hhx ", data_recv[i]);
        }
        printf(" ) socketCount = (%d)\n", socketCount);
        //write(fd, data_send, strlen(data_send));
        //saveMessage(data_recv, i_recvBytes);
        if (i_recvBytes < 1)
        {
            errcount++;
            if (errcount >= 100)
            {
                printf("datalength error\n");
                write(fd, data_recv, i_recvBytes);
                close(fd);
                pthread_detach(pthread_self());
                printf("close a pthread\n");
                break;
            }
        }
        else
        {
            // write(fd, data_recv, i_recvBytes);
            int resNum = readMessage(data_recv, i_recvBytes, fd);
            if (resNum == -1)
            {
                pthread_detach(pthread_self());
                break;
            }
        }
    }
    //Clear
    printf("terminating current client_connection...\n");
    close(fd);          //close a file descriptor.
    pthread_exit(NULL); //terminate calling thread!
}
void *asynRedis(void *arg)
{
    printf("asynRedis run");
    sleep(1);
    asyn();
    pthread_exit(NULL);
}
void *socketStart(void *arg)
{
    printf("socketStart run");
    sleep(1);
    socket_run();
    pthread_exit(NULL);
}
int runThread()
{
    //socket_run();
    int res;
    pthread_t a_thread;
    pthread_t b_thread;
    void *thread_result;
    //开启redis订阅
    res = pthread_create(&a_thread, NULL, asynRedis, NULL);
    //开启socket监听
    res = pthread_create(&b_thread, NULL, socketStart, NULL);
    res = pthread_join(a_thread, NULL);
    res = pthread_join(b_thread, NULL);
    return 0;
}

char *getDevBySlave(int slave)
{
    if (slave > dma.size)
    {
        return NULL;
    }
    return dma.dma[slave]->dev;
}

char getTypeByFun(int fun)
{
    switch (fun)
    {
    case 1:
        return '4';
        break;
    case 2:
        return '3';
        break;
    case 3:
        return '2';
        break;
    case 4:
        return '0';
        break;
    default:
        return 0;
        break;
    }
    return 0;
}
char *getKeyBySlavePoint(modbus_request *mrq)
{
    //char *dev, char type, int point
    char *dev = getDevBySlave(mrq->slave);
    char type = getTypeByFun(mrq->fun);
    int point= mrq->reg_str;
    char devtype[8];
    memset(devtype, 0, 8);
    sprintf(devtype, "%s%c??", dev, type);
    Keys keys;
    keys.dev = devtype;
    getKeys(&keys);

    char *key;
    if (keys.size >= point)
    {
        key = strdup(keys.keys[point]);
    }
    else
    {
        key = strdup("");
    }
    freeKeys(&keys);

    return key;
}


int fun01(modbus_request *mrq, char *resdata) //BO
{
    //mrq->reg_str = mrq->reg_str + 1;
    printf("fun(%d)  slave=(%d) reg_str=(%d) reg_num=(%d)\n", mrq->fun, mrq->slave, mrq->reg_str, mrq->reg_num);
    //DeviceMemory dm = DeviceMemorys[mrq->slave - 1][0];

    DeviceMemory dm = dma.dma[mrq->slave - 1][0];
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

    printf("\n str = (%s) ", str);
    return send(mrq->conn, resdata, 8 + resdata[8] + 1, 0);
}
int fun02(modbus_request *mrq, char *resdata) //BI
{
    return fun01(mrq, resdata);
}

int fun04(modbus_request *mrq, char *resdata)
{
    return fun03(mrq, resdata);
}
int fun03(modbus_request *mrq, char *resdata) //AV
{
    printf("fun%d  slave=(%d) reg_str=(%d) reg_num=(%d)\n", mrq->fun, mrq->slave, mrq->reg_str, mrq->reg_num);
    //DeviceMemory dm = DeviceMemorys[mrq->slave - 1][0];

    DeviceMemory dm = dma.dma[mrq->slave - 1][0];
    int start = mrq->reg_str;
    int end = mrq->reg_num;
    int count = 0;
    int i;
    printf("value = (");
    for (i = start; i < end + start; i++)
    {
        union {
            uint8_t byte[8];
            float real_value;
        } my_data;
        if (mrq->fun == 3)
        {
            printf("%f, ", dm.AV[i]);
            my_data.real_value = dm.AV[i];
        }
        else
        {
            printf("%f, ", dm.AI[i]);
            my_data.real_value = dm.AI[i];
        }
        resdata[9 + count * 4] = my_data.byte[3];
        resdata[10 + count * 4] = my_data.byte[2];
        resdata[11 + count * 4] = my_data.byte[1];
        resdata[12 + count * 4] = my_data.byte[0];
        count++;
    }
    printf(")\n");

    printf("resdata=(");
    for (i = 0; i < 8 + resdata[8] + 1; i++)
    {
        printf("%02hhx ", resdata[i]);
    }
    printf(");\n");
    return send(mrq->conn, resdata, 8 + resdata[8] + 1, 0);
}

int fun06(modbus_request *mrq, char *resdata)
{
    //return fun03(mrq, resdata);
    return 0;
}
int fun15(modbus_request *mrq, char *resdata)
{

    //return fun03(mrq, resdata);
    return 0;
}
int fun16(modbus_request *mrq, char *resdata)
{

    //return fun03(mrq, resdata);
    return 0;
}
int fun05(modbus_request *mrq, char *resdata)
{
    // 0c 50 00 00 00 06 01 05 00 00 ff 00
    char *key = getKeyBySlavePoint(mrq);
    printf("key = (%s)\n", key);
    return send(mrq->conn, mrq->buffer, mrq->bufferlen, 0);
}
//根据报文返回数据
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
    if (mrq.reg_str > dma.size)
    {
        printf("reg_str max is %d \n", dma.size);
        buffer[7] = buffer[7] + 80;
        send(conn, buffer, len, 0);
        return 1;
    }

    mrq.reg_num = reg_num; //数量
    if (mrq.reg_num > 20)
    {
        printf("regnum max is 20");
        mrq.reg_num = 20;
    }
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
    if (mrq.slave - 1 > dma.size)
    {
        printf("error slave is not found (%d) dma.size=(%d)\n ", mrq.slave, dma.size);
        buffer[7] = buffer[7] + 80;
        send(conn, buffer, len, 0);
        return 1;
    }
    if (mrq.reg_str + mrq.reg_num > 512)
    {
        buffer[7] = buffer[7] + 80;
        printf("error key stackoverflow (%d) ", mrq.reg_str + mrq.reg_num);
        send(conn, buffer, len, 0);
        return 1;
    }
    updateModuleAddSlave(mrq.slave);
    switch (mrq.fun)
    {
    case 1:
        return fun01(&mrq, resdata);
        break;
    case 2:
        return fun02(&mrq, resdata);
        break;
    case 3:
        return fun03(&mrq, resdata);
        break;
    case 4:
        return fun04(&mrq, resdata);
        break;
    case 5:
        return fun05(&mrq, resdata);
    case 6:
        return fun06(&mrq, resdata);
    case 15:
        return fun15(&mrq, resdata);
    case 16:
        return fun16(&mrq, resdata);
    default:
        saveMessage(buffer, len);
        return send(conn, resdata, 8, 0);
        break;
    }

    return 0;
}
int saveMessage(char *buffer, int len)
{
    //    int fputc( int c, FILE *fp );

    FILE *fp;
    int i;
    fp = fopen("message.text", "a+");
    for (i = 0; i < len; i++)
    {
        fprintf(fp, "%02hhx ", buffer[i]);
    }
    fputc('\n', fp);
    //fputs(buffer, fp);
    fclose(fp);
    return 0;
}

//回收站------------------------------------------------------------------------------------------------------------------------------
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

int test()
{
    Keys keys;
    keys.dev = "11018??\n";
    getKeys(&keys);
    freeKeys(&keys);
    // Devs devs;
    // getDevs(&devs);
    // freeDevs(&devs);
    return 0;
}