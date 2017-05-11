#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include "hiredis/hiredis.h"
#include "mxml-release-2.10/mxml.h"
static const char *netnum;
int initNetNum();
int initNetNum()
{
    FILE *fp;
    mxml_node_t *tree;
    fp = fopen("/mnt/nandflash/bac_config.xml", "r");
    if (fp == 0)
    {
        printf("/mnt/nandflash/bac_config.xml not found");
        return 1;
    }
    tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    mxml_node_t *node;
    node = mxmlFindElement(tree, tree, "net", NULL, NULL, MXML_DESCEND);
    netnum = mxmlGetText(node, 0);
    printf("%s", netnum);
    fclose(fp);
    return 0;
}

int main()
{

    //printf("%d",initNetNum());

    modbus_new_tcp("127.0.0.1", 1502);
    redisContext *c = redisConnect("127.0.0.1", 6379);
    const char* command1 = "set stest1 value1";
    redisReply* r = (redisReply*)redisCommand(c, command1);

    // redisFree(c);
    return 0;
}
