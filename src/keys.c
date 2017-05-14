#include "../include/keys.h"
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

    redisContext *c = redisConnect("127.0.0.1", 6379);
    char *command = (char *)malloc(100);
    strcat(command, "keys ");
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

    redisReply *reply = (redisReply *)redisCommand(c, command);
    int len = reply->elements;
    keys->keys = (char **)calloc(reply->elements, sizeof(char *));

    keys->size = len;
    int i;
    for (i = 0; i < len; i++)
    {
        keys->keys[i] = strdup(reply->element[i]->str);
    }
    sortKeys(keys, len - 1);
    for (i = 0; i < len; i++)
    {
        printf("key%d=(%s) ", i, keys->keys[i]);
    }
    printf("\n");
    redisFree(c);
    return 0;
}
int getDevs(deviceSizeStrs *sdevs)
{
    // redisContext *c = redisConnect("127.0.0.1", 6379);
    // const char *command1 = "keys ???????";
    // redisReply *reply = (redisReply *)redisCommand(c, command1);
    Keys keys;
    keys.dev = NULL;
    getKeys(&keys);

    char *devs[keys.size];
    int i, count = 0;
    for (i = 0; i < keys.size; i++)
    {
        char *str = keys.keys[i];
        char *devnum = (char *)malloc(5);
        strncat(devnum, str, 4);
        devs[i] = devnum;
    }
    count = Unique(devs, keys.size);
    sdevs->size = count;
    sdevs->devs = (char **)calloc(count, sizeof(char *));
    for (i = 0; i < count; i++)
    {
        sdevs->devs[i] = strdup(devs[i]);
        printf("dev%d=(%s) ", i, sdevs->devs[i]);
    }
    return 0;
}
int getDeviceMemory(DeviceMemory *dm, char *dev)
{
    redisContext *c = redisConnect("127.0.0.1", 6379);

    int i, types = 6;
    for (i = 0; i < types; i++)
    {
        char *command = (char *)malloc(10);
        sprintf(command, "%s%d%s", dev, i, "??");
        Keys keys;
        keys.dev = command;
        getKeys(&keys);
        int j;
        printf("findkey = (%s) size = (%d)\n", command, keys.size);

        for (j = 0; j < keys.size; j++)
        {
            char *command1 = (char *)malloc(30);
            sprintf(command1, "%s %s %s", "hget", keys.keys[j], "Present_Value");
            redisReply *reply = (redisReply *)redisCommand(c, command1);
            printf("command1 = (%s) value = (%s)", command1, reply->str);
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
        }

        printf("end   \n");
        // int j;
        // for (j = 0; j < reply->elements; j++)
        // {
        //     printf("--value =  %s --\n", reply->element[j]->str);
        // }
    }
    redisFree(c);

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

// int main()
// {
//     //float **b = calloc(502, sizeof(float));
//     float value = 125.213;
//     union {
//         uint8_t byte[8];
//         float real_value;
//     } my_data;
//     my_data.real_value = value;

//     my_data.real_value = value;
//     //float a = c>>8;
//     // deviceSizeStrs dSizeStrs;
//     // int a = getDevs(&dSizeStrs);
//     // Keys keys;
//     // keys.dev = "10011??";
//     // getKeys(&keys);

//     DeviceMemoryInit();

//     return 0;
// }
int DeviceMemoryInit()
{
    deviceSizeStrs devs;
    getDevs(&devs);
    DeviceMemory **dms = (DeviceMemory **)calloc(devs.size, sizeof(DeviceMemory));
    int i;
    for (i = 0; i < devs.size; i++)
    {
        DeviceMemory *a = (DeviceMemory *)malloc(sizeof(DeviceMemory));
        getDeviceMemory(a, devs.devs[i]);
        dms[i] = a;
    }
    printf(" %s ", " devs.devs[i]");
    DeviceMemorys = dms;
    return 0;
}