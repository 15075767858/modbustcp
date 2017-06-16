#include "main.h"
#include <unistd.h>
int sockfd_server;

int main()
{

    error_quit();
    //初始化redis
    redisInit();
    printf("redisInit\n");
    //初始化全局所有key，方便后面用不再从数据库查询
    initKeysAll();
    printf("initKeysAll\n");
    //初始化全局所有device，方便后面用不再从数据库查询
    initDevsAll();
    printf("initDevsAll\n");
    //初始化全局所有map_key，方便后面用不再从数据库查询
    //initMapKeys();
    //printf("initMapKeys\n");
    initDeviceByXml();
    printf("initDeviceByXml\n");
    //while(1)
    updateXmlMapKeys();
    printf("updateXmlMapKeys\n");
    //initUpdateModule();
    //初始化设备内存页
    //initDeviceMemoryAll();
    //初始化设备内存
    //initDeviceXml();
    //开启10秒一次更新内存页
    //开启多线程访问
    runThread();
    printf("asyn redis run\n");
    while (1)
    {
    }
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
        port = SOCK_PORT;
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
        printf("waiting for new connection... ");
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
        //printf("%s", thread_id);
        pthread_detach(thread_id);
    }
    //Clear
    int ret = shutdown(sockfd_server, SHUT_WR); //shut down the all or part of a full-duplex connection.
    assert(ret != -1);
    printf("Server shuts down\n");
    return 0;
}
static void my_close_socket(int sig)
{ // can be called asynchronously
    close(sockfd_server);

    shutdown(sockfd_server, SHUT_RDWR);

    printf("shutdown");
    exit(0);
}
void error_quit()
{
    signal(1, my_close_socket);
    signal(2, my_close_socket);
    signal(3, my_close_socket);
    signal(4, my_close_socket);
    signal(5, my_close_socket);
    signal(6, my_close_socket);
    signal(8, my_close_socket);
    signal(9, my_close_socket);
    signal(10, my_close_socket);
    signal(11, my_close_socket);
    signal(12, my_close_socket);
    signal(13, my_close_socket);
    signal(15, my_close_socket);
    signal(16, my_close_socket);
    signal(17, my_close_socket);
    signal(18, my_close_socket);
    signal(19, my_close_socket);
    signal(20, my_close_socket);
    signal(21, my_close_socket);
    signal(22, my_close_socket);
    signal(23, my_close_socket);
    signal(24, my_close_socket);
    signal(25, my_close_socket);
}
static void Data_handle(void *sock_fd)
{

    int fd = *((int *)sock_fd);
    int i_recvBytes;
    char data_recv[BUFFER_LENGTH];
    int errcount = 0;
    while (1)
    {
        //pthread_detach(pthread_self());
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
                //pthread_detach(pthread_self());
                pthread_exit(NULL);
                printf("close a pthread\n");
                break;
            }
        }
        else
        {
            // write(fd, data_recv, i_recvBytes);
            readMessage(data_recv, i_recvBytes, fd);
            // printf("resNum = %d \n", resNum);
            // if (resNum == -1)
            // {
            //     pthread_detach(pthread_self());
            //     break;
            // }
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
void *DeviceMemoryAllUpdateStart(void *arg)
{
    printf("DeviceMemoryAllUpdateStart run");
    sleep(1);
    signal(SIGALRM, signal_handler);
    set_timer();
    pthread_exit(NULL);
}
int runThread()
{
    int res;
    pthread_t a_thread;
    pthread_t b_thread;
    pthread_t c_thread;
    void *thread_result;
    //开启redis订阅
    res = pthread_create(&a_thread, NULL, asynRedis, NULL);
    printf("asynRedis\n");
    //开启socket监听
    res = pthread_create(&b_thread, NULL, socketStart, NULL);
    printf("socketStart\n");
    //开启轮询数据库
    res = pthread_create(&c_thread, NULL, DeviceMemoryAllUpdateStart, NULL);
    printf("DeviceMemoryAllUpdateStart\n");
    pthread_detach(a_thread);
    pthread_detach(b_thread);
    pthread_detach(c_thread);
    //res = pthread_join(a_thread, NULL);
    //res = pthread_join(b_thread, NULL);
    //res = pthread_join(c_thread, NULL);
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
        return '1';
        break;
    case 4:
        return '0';
        break;
    case 5:
        return '4';
        break;
    case 16:
        return '1';
    default:
        return 0;
        break;
    }
    return 0;
}
char *getKeyBySlavePoint(modbus_request *mrq)
{

    //char *dev, char type, int point
    char *dev = getDevBySlave(mrq->slave - 1);
    char type = getTypeByFun(mrq->fun);

    int point = mrq->reg_str;
    if (mrq->fun == 16)
    {
        point = point / 2;
    }

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
    printf("fun5 key=%s\n", key);
    return key;
}

int fun01(modbus_request *mrq, char *resdata) //BO
{
    //mrq->reg_str = mrq->reg_str + 1;

    printf("slave=(%d)   fun(%d)   reg_str=(%d) reg_num=(%d)\n", mrq->slave, mrq->fun, mrq->reg_str, mrq->reg_num);
    //DeviceMemory dm = DeviceMemorys[mrq->slave - 1][0];
    mrq->reg_num = mrq->reg_num + 1;
    //DeviceMemory dm = dma.dma[mrq->slave - 1][0];
    int start = mrq->reg_str;
    int end = mrq->reg_num;
    int count = 0;
    int i;
    char str[mrq->reg_num + 1];
    int jishu = 0;
    memset(str, 0, mrq->reg_num);
    char type;
    if (mrq->fun == 1)
    {
        type = '4';
    }
    else
    {
        type = '3';
    }
    xml_map_key *resxmk;
    int byte8[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};
    unsigned char byte1 = 0x0;

    for (i = start; i < end + start; i++)
    {
        if (count % 8 == 0)
        {
            byte1 = 0x0;
        }
        resxmk = findXMKByXmlMapKey(mrq->slave, i + 1, type);
        if (resxmk != NULL)
        {
            if (atoi(resxmk->value) != 0)
            {
                byte1 += byte8[count];
            }
        }
        resdata[9 + count / 8] = byte1;

        count++;
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
    int start = mrq->reg_str;
    int end = mrq->reg_num;
    int count = 0;
    int i;
    printf("value = (");
    char type;
    if (mrq->fun == 3)
    {
        type = '1';
    }
    else
    {
        type = '0';
    }

    for (i = start; i < end + start; i++)
    {
        union {
            uint8_t byte[8];
            float real_value;
        } my_data;
        xml_map_key *resxmk = findXMKByXmlMapKey(mrq->slave, i + 1, type);
        if (resxmk != NULL)
        {
            printf("key = (%s) slave = (%d) point = (%d) value = (%s) ", resxmk->key, resxmk->slave, resxmk->point, resxmk->value);
            my_data.real_value = atof(resxmk->value);
        }
        else
        {
            my_data.real_value = 0;
        }
        resdata[9 + count * 4] = my_data.byte[3];
        resdata[10 + count * 4] = my_data.byte[2];
        resdata[11 + count * 4] = my_data.byte[1];
        resdata[12 + count * 4] = my_data.byte[0];
        count++;
    }
    printf(")\n");
    int resNum = count * 2+2;
    resdata[4] = (resNum + 3) >> 8;
    resdata[5] = (resNum + 3) & 0x00FF;
    resdata[8] = (unsigned char)resNum;
    int resLen = 8 + resNum + 1;
    printf("response=(");
    for (i = 0; i < resLen; i++)
    {
        printf("%02hhx ", resdata[i]);
    }
    printf(");\nresLen = %d resNum= %d resdata[8]=%d \n", resLen, resNum, resdata[8]);

    return send(mrq->conn, resdata, resLen, 0);
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
    //char *key = getKeyBySlavePoint(mrq);
    xml_map_key *xmk = findXMKByXmlMapKey(mrq->slave, mrq->reg_str + 1, '1');
    if (xmk != NULL)
    {
        return send(mrq->conn, mrq->buffer, mrq->bufferlen, 0);
    }
    char *key = xmk->key;
    union {
        uint8_t byte[8];
        float real_value;
    } my_data;
    my_data.byte[3] = mrq->buffer[13];
    my_data.byte[2] = mrq->buffer[14];
    my_data.byte[1] = mrq->buffer[15];
    my_data.byte[0] = mrq->buffer[16];
    char sVal[20];
    memset(sVal, 0, 20);
    sprintf(sVal, "%f", my_data.real_value);
    redisSetValue(redis, key, "Present_Value", sVal);
    changePriority(redis, key, sVal, 7);
    printf("key = %s real_value = %f\n", key, my_data.real_value);
    //return fun03(mrq, resdata);
    return send(mrq->conn, mrq->buffer, mrq->bufferlen, 0);
}
int fun05(modbus_request *mrq, char *resdata)
{
    // 0c 50 00 00 00 06 01 05 00 00 ff 00
    //char *key = getKeyBySlavePoint(mrq);
    xml_map_key *xmk = findXMKByXmlMapKey(mrq->slave, mrq->reg_str + 1, '4');
    if (xmk == NULL)
    {
        return send(mrq->conn, mrq->buffer, mrq->bufferlen, 0);
    }
    char *key = xmk->key;
    if (mrq->buffer[10] == 0)
    {
        redisSetValue(redis, key, "Present_Value", "0");
        changePriority(redis, key, "0", 7);
    }
    else
    {
        redisSetValue(redis, key, "Present_Value", "1");
        changePriority(redis, key, "1", 7);
    }

    //free(key);
    printf("change key = (%s)\n", key);
    return send(mrq->conn, mrq->buffer, mrq->bufferlen, 0);
}
//根据报文返回数据
int readMessage(char *buffer, int len, int conn)
{

    int reg_num = buffer[10] * 256 + buffer[11] - 1;
    modbus_request mrq;
    //mrq.transaction = buffer[0] * 256 + (u8)buffer[1];
    //mrq.protocol = buffer[2] * 256 + buffer[3];
    mrq.len = buffer[4] * 256 + buffer[5];
    mrq.slave = buffer[6];                     //设备地址
    mrq.fun = (int)buffer[7];                  //功能码
    mrq.reg_str = buffer[8] * 256 + buffer[9]; //点位编号
    mrq.reg_num = reg_num;
    // if (mrq.fun == 1 || mrq.fun == 2 || mrq.fun == 3 || mrq.fun == 4)
    // {
    // }

    mrq.buffer = buffer;
    mrq.bufferlen = len;
    mrq.conn = conn;
    char resdata[BUFFER_LENGTH];
    memset(resdata, 0, BUFFER_LENGTH);
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
    // case 6:
    //     return fun06(&mrq, resdata);
    // case 15:
    //     return fun15(&mrq, resdata);
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