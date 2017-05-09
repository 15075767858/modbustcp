#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void test(char *buf)
{
    buf[0] = '1';
    buf[1] = '1';
    buf[2] = '1';
}

void catNumAdd0(char *buffer, int num)
{
    char buf[10];
    memset(buf, 0, 10);
    sprintf(buf, "%02d", num);
    strcat(buffer, buf);
}

int main()
{
    char s[] = "11.1";
    int a = atoi(s);

    printf("%d  \n", a);
    //printf("%Lf\n", a);
    //test(buf);
    //printf("%s \n",buf);

    //test();
    // int num = 10;

    // char buf[10];
    // memset(buf, 0, 10);
    // if (num < 10)
    // {
    //     buf[0] = '0';
    // }
    // sprintf(buf, "%d", num);
    // char *str = buf;
    // printf("%s \n", str);

    // modbus_new_tcp("127.0.0.1", 1502);
    // redisContext *c = redisConnect("127.0.0.1", 6379);
    // const char* command1 = "set stest1 value1";
    // redisReply* r = (redisReply*)redisCommand(c, command1);

    // redisFree(c);
    return 0;
}
