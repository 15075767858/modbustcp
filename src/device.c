
#include "device.h"
#include <sys/utsname.h>
// #include "../memwatch-2.71/memwatch.h"

// #ifndef SIGSEGV
// #error "SIGNAL.H does not define SIGSEGV; running this program WILL cause a core dump/crash!"
// #endif

// #ifndef MEMWATCH
// #error "You really, really don't want to run this without memwatch. Trust me."
// #endif

// #if !defined(MW_STDIO) && !defined(MEMWATCH_STDIO)
// #error "Define MW_STDIO and try again, please."
// #endif

int redisInit()
{
    keysredis = redisConnect("127.0.0.1", 6379);
    redis = redisConnect("127.0.0.1", 6379);
    memoryredis = redisConnect("127.0.0.1", 6379);
    int keyLen = 0;

    // while (1)
    // {
    //     sleep(15);
    //     redisReply *reply = (redisReply *)redisCommand(keysredis, "keys *");
    //     if (keyLen == reply->elements)
    //     {
    //         break;
    //     }
    //     keyLen = reply->elements;
    //     printf("keyLen=(%d)", keyLen);
    //     freeReplyObject(reply);
    // }
    //err 1 errstr[128]
    // if (redis->err != 0)
    // {
    //     printf("redis %s \n", redis->errstr);
    //     exit(0);
    //     return redis->err;
    // }
    //redisFree(redis);

    return 0;
}

int initMapKeys()
{
    int i, count = 0;
    mks.size = keysAll.size;
    mks.mks = calloc(keysAll.size, sizeof(map_key));
    Keys keys;

    for (i = 0; i < devsAll.size; i++)
    {
        keys.dev = devsAll.devs[i];
        getKeysForKeysAll(&keys);
        int j;
        for (j = 0; j < keys.size; j++)
        {
            char *key = keys.keys[j];
            map_key *mk = malloc(sizeof(map_key));
            mks.mks[count] = mk;
            sprintf(mk->key, "%s", key);
            sscanf(key, "%4s%1s%2s", mk->dev, mk->type, mk->number);
            mk->slave = i + 1;
            mk->point = j + 1;
            count++;
        }
        free(keys.keys);
    }
    printf("initMapKeys ok\n");
    return 0;
}
//根据key获取Object_Name
char *getKeyObjectName(char *key)
{
    char command[50];
    memset(command, 0, 50);
    sprintf(command, "hget %s Object_Name", key);
    redisReply *reply = (redisReply *)redisCommand(keysredis, command);
    if (reply->str != 0)
    {
        char *ObjectName = strdup(reply->str);
        freeReplyObject(reply);
        return ObjectName;
    }
    else
    {
        return strdup("");
    }
}

int redisSetValue(redisContext *redis, char *key, char *property, char *value)
{
    char dev[5];
    memset(dev, 0, 5);
    strncat(dev, key, 4);
    char valCommand[1000];
    memset(valCommand, 0, 1000);
    char pubCommand[1000];
    memset(pubCommand, 0, 1000);
    sprintf(valCommand, "hset %s %s %s", key, property, value);
    redisReply *reply;
    reply = (redisReply *)redisCommand(redis, valCommand);
    freeReplyObject(reply);
    sprintf(pubCommand, "PUBLISH %s.8.* %s\r\n%s\r\n%s", dev, key, property, value);
    reply = (redisReply *)redisCommand(redis, pubCommand);
    freeReplyObject(reply);
    return 0;
}
char *redisGetValue(redisContext *redis, char *key, char *property)
{
    char valCommand[1000];
    memset(valCommand, 0, 1000);
    sprintf(valCommand, "hget %s %s ", key, property);
    redisReply *reply;
    reply = (redisReply *)redisCommand(redis, valCommand);
    char *value;
    if (reply->str != 0)
    {
        value = strdup(reply->str);
    }
    else
    {
        value = strdup("");
    }
    freeReplyObject(reply);
    return value;
}
int changePriority(redisContext *redis, char *key, char *value, int priority)
{
    char *PriArray = redisGetValue(redis, key, "Priority_Array");
    char arrs[16][10] = {"NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"};

    if (strlen(PriArray) < 59)
    {
        int i = 0;
        char *token = strtok(PriArray, ",");
        while (token != NULL)
        {
            if (print == 0)
                printf(" %s ", token);
            memset(arrs[i], 0, 10);
            if (i == priority)
            {
                sprintf(arrs[i], "%s", value);
            }
            else
            {
                sprintf(arrs[i], "%s", token);
            }
            i++;
            token = strtok(NULL, ",");
        }
    }
    else
    {
        memset(arrs[priority], 0, 10);
        sprintf(arrs[priority], "%s", value);
    }
    char pValue[100];
    memset(pValue, 0, 100);
    sprintf(pValue, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s", arrs[0], arrs[1], arrs[2], arrs[3], arrs[4], arrs[5], arrs[6], arrs[7], arrs[8], arrs[9], arrs[10], arrs[11], arrs[12], arrs[13], arrs[14], arrs[15]);
    redisSetValue(redis, key, "Priority_Array", pValue);
    if (print == 0)
        printf("\nend %s\n", pValue);
    free(PriArray);
    return 0;
}
//根据dev获取dev的index
int getDevIndex(char *dev)
{
    int i;
    for (i = 0; i < dma.size; i++)
    {
        if (strncmp(dev, dma.dma[i]->dev, 4) == 0)
        {
            return i;
        }
    }
    return -1;
}
//根据key获取key的index
int getKeyIndex(char *key)
{
    char qKey[7];
    memset(qKey, 0, 7);
    strncpy(qKey, key, 5);
    Keys keys;
    keys.dev = qKey;
    getKeysForKeysAll(&keys);
    //getKeys(&keys);
    int i;
    for (i = 0; i < keys.size; i++)
    {
        if (strncmp(key, keys.keys[i], 7) == 0)
        {
            free(keys.keys);
            //freeKeys(&keys);
            return i;
        }
    }
    free(keys.keys);
    //freeKeys(&keys);
    return -1;
}

//自动更新内存页
void signal_handler(int m)
{
    updateXmlMapKeys();
    //DeviceMemoryAllUpdate();

    // jsqCount++;
    // if (jsqCount % 6 == 0)
    // {
    //     updateKeysAll();
    //     printf("jsqCount = %ld\n", jsqCount);
    // }
}
//定时器
void set_timer()
{
    struct itimerval itv;
    itv.it_interval.tv_sec = 10;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itv, &oldtv);
}

//初始化一个全局keys
int initKeysAll()
{
    getKeys(&keysAll);
    return 0;
}
int updateKeysAll()
{
    Keys keys;
    keys.dev = NULL;
    getKeys(&keys);
    free(keysAll.keys);
    keysAll.keys = keys.keys;
    keysAll.size = keys.size;
    return 0;
}
void updateXmlMapKeys()
{
    int i;
    for (i = 0; i < xmks.size; i++)
    {
        char *value = redisGetValue(redis, xmks.xmks[i]->key, "Present_Value");
        printf("%s %s ", xmks.xmks[i]->key, value);
        memset(xmks.xmks[i]->value, 0, 20);

        sprintf(xmks.xmks[i]->value, "%s", value);
        //        strncat(xmks.xmks[i]->value, value, 20);
        free(value);
    }
}

xml_map_key *findXMKByXmlMapKey(int slave, int point, char pointType)
{
    switch (pointType)
    {
    case '0':
        point += aioffset;
        break;
    case '1':
        point += aooffset;
        break;
    case '3':
        point += dioffset;
        break;
    case '4':
        point += dooffset;
        break;
    }
    
    int i;
    for (i = 0; i < xmks.size; i++)
    {
        if (xmks.xmks[i]->slave == slave & xmks.xmks[i]->point == point & xmks.xmks[i]->pointType[0] == pointType)
        {

            return xmks.xmks[i];
        }
    }

    return NULL;
}

int initDevsAll()
{
    getDevs(&devsAll);
    return 0;
}
//从全局keys中获取要查找的key
int getKeysForKeysAll(Keys *keys)
{
    int keylen = strlen(keys->dev);
    int i, count = 0;
    for (i = 0; i < keysAll.size; i++)
    {
        if (strncmp(keys->dev, keysAll.keys[i], keylen) == 0)
        {
            count++;
        }
    }
    keys->size = count;
    keys->keys = (char **)calloc(count, sizeof(char *));
    count = 0;
    for (i = 0; i < keysAll.size; i++)
    {
        if (strncmp(keys->dev, keysAll.keys[i], keylen) == 0)
        {
            keys->keys[count] = keysAll.keys[i];
            count++;
        }
    }
    return 0;
}
//获取key
int getKeys(Keys *keys)
{
    char command[100];
    memset(command, 0, 100);
    strcat(command, "keys \0");
    redisReply *reply;
    if (keys->dev != NULL)
    {
        if (print == 0)
            printf("dev=(%s)", keys->dev);
        strcat(command, keys->dev);
    }
    else
    {
        strcat(command, "????[012345]??\0");
        if (print == 0)
            printf("\n  command = (%s) \n", command);
    }
    reply = (redisReply *)redisCommand(keysredis, command);
    int len = reply->elements;
    keys->keys = (char **)calloc(len, sizeof(char *));
    int i;
    for (i = 0; i < len; i++)
    {
        keys->keys[i] = strdup(reply->element[i]->str);
    }
    keys->size = i;

    sortKeys(keys, keys->size - 1);
    freeReplyObject(reply);
    //redisFree(c);

    return 0;
}
//获取dev
int getDevs(Devs *sdevs)
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
        sdevs->devs[i] = strdup(devs[i]);
        //sdevs->devs[i] = devs[i];
        if (print == 0)
            printf("dev%d=(%s) ", i, sdevs->devs[i]);
    }
    freeKeys(&keys);
    return 0;
}

int getDeviceMemory(DeviceMemory *dm, char *dev)
{
    int i, types = 6;
    for (i = 0; i < types; i++)
    {
        char command[10];
        memset(command, 0, 10);
        sprintf(command, "%s%d", dev, i);
        Keys keys;
        keys.dev = command;
        getKeysForKeysAll(&keys);
        memset(command, 0, 10);
        int j;
        char command1[30];
        for (j = 0; j < keys.size; j++)
        {
            memset(command1, 0, 30);
            sprintf(command1, "%s %s %s", "hget", keys.keys[j], "Present_Value");
            redisReply *reply = (redisReply *)redisCommand(memoryredis, command1);
            if (reply == 0)
            {

                printf("error memoryredis --------------------------------\n");
                //    return 0;
            }
            if (reply->str == 0)
            {
                reply->str = "0\0";
            }
            if (print == 0)
                printf("key%s = %s \n", keys.keys[j], reply->str);
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
        free(keys.keys);
        //freeKeys(&keys);
    }

    return 0;
}

//初始化device内存
int initDeviceMemoryAll()
{
    dma.dma = (DeviceMemory **)calloc(devsAll.size, sizeof(DeviceMemory));
    dma.size = devsAll.size;
    int i;
    for (i = 0; i < devsAll.size; i++)
    {
        DeviceMemory *dm = (DeviceMemory *)malloc(sizeof(DeviceMemory)); //获得一个设备 所有的5个点
        sprintf(dm->dev, "%s", devsAll.devs[i]);
        getDeviceMemory(dm, devsAll.devs[i]);
        dma.dma[i] = dm;
    }
    return 0;
}
//device内存更新
int DeviceMemoryAllUpdate()
{
    int i;
    dma.size = devsAll.size;
    for (i = 0; i < devsAll.size; i++)
    {
        //        if (updateModuleIsHaveSlave(i + 1) == 0)
        //        {
        //        }
        if (print == 0)
            printf("updateModel =(%d)", i);
        DeviceMemory *dm = (DeviceMemory *)malloc(sizeof(DeviceMemory)); //获得一个设备 所有的5个点
        memset(dm, 0, sizeof(DeviceMemory));
        memset(dm->dev, 0, 4);
        strncat(dm->dev, devsAll.devs[i], 4);
        getDeviceMemory(dm, devsAll.devs[i]);
        free(dma.dma[i]);
        dma.dma[i] = dm;
        //usleep(9000000 / devsAll.size);
    }
    return 0;
}

int initUpdateModule()
{
    memset(uM.slaves, 0, sizeof(int) * 100);
    return 0;
}
int updateModuleAddSlave(int slave)
{
    int i;
    for (i = 0; i < 100; i++)
    {
        if (uM.slaves[i] == slave)
        {
            break;
        }
        if (uM.slaves[i] == 0)
        {
            uM.size += 1;
            uM.slaves[i] = slave;
            break;
        }
    }

    return 0;
}
int updateModuleIsHaveSlave(int salve)
{
    printf("module slave =(");
    int i;
    for (i = 0; i < uM.size; i++)
    {
        printf("%d  ", uM.slaves[i]);
        if (uM.slaves[i] == salve)
        {
            return 0;
        }
    }
    printf(" ) \n");
    return i;
}
//数字转字符串
void catNumAdd0(char *buffer, int num)
{
    char buf[10];
    memset(buf, 0, 10);
    sprintf(buf, "%02d", num);
    strcat(buffer, buf);
}
//缓冲区增加字节
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
}
//字符串转数字
int bit8ToInt(char *strbuf)
{
    char resbuf[9];

    memset(resbuf, 0, 9);
    resbuf[0] = strbuf[7];
    resbuf[1] = strbuf[6];
    resbuf[2] = strbuf[5];
    resbuf[3] = strbuf[4];
    resbuf[4] = strbuf[3];
    resbuf[5] = strbuf[2];
    resbuf[6] = strbuf[1];
    resbuf[7] = strbuf[0];
    resbuf[8] = '\0';
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
//数字转字符串
char *intToChar(int num)
{
    char numbuf[100];
    memset(numbuf, 0, 100);
    if (print == 0)
        sprintf(numbuf, "%d", num);
    char *aa = numbuf;
    return aa;
}
//devs去重
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
//释放devs内存
int freeDevs(Devs *devs)
{
    int i;
    for (i = 0; i < devs->size; i++)
    {
        free(devs->devs[i]);
    }
    free(devs->devs);
    return 0;
}
//释放keys内存
int freeKeys(Keys *keys)
{
    int i;
    for (i = 0; i < keys->size; i++)
    {
        if (keys->keys[i] != 0)
        {
            free(keys->keys[i]);
        }
    }
    if (keys->keys != 0)
    {

        free(keys->keys);
    }
    return 0;
}
//key排序
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
