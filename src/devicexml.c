#include "devicexml.h"
#define XMLFILENAME "modbusID.xml"

int rootAddDevice(mxml_node_t *root);
int deviceAddTypes(mxml_node_t *device);
int deviceTypeAddKey(mxml_node_t *deviceType);
char getTypeNumberByDeviceTypeNode(mxml_node_t *deviceType);

//格式化方法
const char *whitespace_cb(mxml_node_t *node, int where)
{
    const char *name;
    /*
      * We can conditionally break to a new line
      * before or after any element. These are
      * just common HTML elements...
      */
    name = mxmlGetElement(node);

    if (!strcmp(name, "root") ||
        !strcmp(name, "device") ||
        !strcmp(name, "AI") ||
        !strcmp(name, "AV") ||
        !strcmp(name, "BI") ||
        !strcmp(name, "BO") ||
        !strcmp(name, "key"))
    {
        /*
	* Newlines before open and after
        * close...
	*/

        if (where == MXML_WS_BEFORE_OPEN)
            return ("\t");
        else if (where == MXML_WS_AFTER_CLOSE)
            return ("\n");
    }
    /*
      * Return NULL for no added whitespace...
      */
    //return ("\n");
    return (NULL);
}

//初始化xml
int initDeviceXml()
{
    //创建一个xml文件
    mxml_node_t *xml = mxmlNewXML("1.0");
    //新增一个节点root
    mxml_node_t *root = mxmlNewElement(xml, "root");
    rootAddDevice(root);
    //打开文件index.xml
    FILE *fp = fopen(XMLFILENAME, "wb");
    //保存节点到xml文件
    // mxmlSetWrapMargin(50);
    mxmlSaveFile(xml, fp, whitespace_cb);
    fclose(fp);
    //删除节点内存
    mxmlDelete(xml);
    return 0;
}
//root增加device
int rootAddDevice(mxml_node_t *root)
{
    Devs devs;
    getDevs(&devs);
    int i;
    char SlaveAddress[3];
    for (i = 0; i < devs.size; i++)
    {
        mxml_node_t *device = mxmlNewElement(root, "device");
        mxmlElementSetAttr(device, "number", devs.devs[i]);
        memset(SlaveAddress, 0, 3);
        sprintf(SlaveAddress, "%d", i + 1);
        mxmlElementSetAttr(device, "SlaveAddress", SlaveAddress);
        deviceAddTypes(device);
    }

    freeDevs(&devs);
    return 0;
}
//device增加types
int deviceAddTypes(mxml_node_t *device)
{
    const char *deviceNumber = mxmlElementGetAttr(device, "number");
    char keyTypes[4][3] = {"AI", "AV", "BI", "BO"};
    char deviceTypes[4][3] = {"AI", "AV", "DI", "DO"};
    int i;
    for (i = 0; i < 4; i++)
    {
        //增加设备节点
        mxml_node_t *deviceType = mxmlNewElement(device, keyTypes[i]);
        //增加设备点类型
        mxmlElementSetAttr(deviceType, "pointType", deviceTypes[i]);
        //根据设备点类型增加key
        deviceTypeAddKey(deviceType);
    }
    return 0;
}
//devicetypes 增加key
int deviceTypeAddKey(mxml_node_t *deviceType)
{
    //需要查找的key
    char findKey[8];
    memset(findKey, 0, 8);
    const char *device = mxmlElementGetAttr(deviceType->parent, "number");
    const char *deviceTypeName = deviceType->value.opaque;
    char typeNumber = getTypeNumberByDeviceTypeNode(deviceType);
    sprintf(findKey, "%s%c??", device, typeNumber);
    Keys keys;
    keys.dev = findKey;
    getKeys(&keys);
    int i;
    for (i = 0; i < keys.size; i++)
    {
        mxml_node_t *key = mxmlNewElement(deviceType, "key");
        mxmlElementSetAttr(key, "number", keys.keys[i]);
        mxmlElementSetAttr(key, "pointNumber", keys.keys[i]);
        char *ObjectName = getKeyObjectName(keys.keys[i]);
        mxmlNewText(key, 0, ObjectName);
        free(ObjectName);
        //printf("%s\n", keys.keys[i]);
    }
    freeKeys(&keys);
    //printf("%s %s %s\n", device, deviceTypeName, findKey);
    return 0;
}
//根据xml初始化 点位
void initDeviceByXml()
{

    typedef struct
    {
        mxml_node_t **nodes;
        int size;
    } xml_nodes;
    xml_nodes xns;
    xns.nodes = calloc(999 * 4, sizeof(mxml_node_t));
    FILE *fp;
    mxml_node_t *tree;
    mxml_node_t *node;
    fp = fopen("/mnt/nandflash/modbusID.xml", "r");
    tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);

    int ai_map_reg, ao_map_reg, av_map_reg, bi_map_reg, bo_map_reg, bv_map_reg;
    node = mxmlFindElement(tree, tree, "ai_map_reg", NULL, NULL, MXML_DESCEND);
    ai_map_reg = atoi(mxmlGetText(node, 0));
    node = mxmlFindElement(tree, tree, "ao_map_reg", NULL, NULL, MXML_DESCEND);
    ao_map_reg = atoi(mxmlGetText(node, 0));
    node = mxmlFindElement(tree, tree, "av_map_reg", NULL, NULL, MXML_DESCEND);
    av_map_reg = atoi(mxmlGetText(node, 0));
    node = mxmlFindElement(tree, tree, "bi_map_reg", NULL, NULL, MXML_DESCEND);
    bi_map_reg = atoi(mxmlGetText(node, 0));
    node = mxmlFindElement(tree, tree, "bo_map_reg", NULL, NULL, MXML_DESCEND);
    bo_map_reg = atoi(mxmlGetText(node, 0));
    node = mxmlFindElement(tree, tree, "bv_map_reg", NULL, NULL, MXML_DESCEND);
    bv_map_reg = atoi(mxmlGetText(node, 0));

    node = mxmlFindElement(tree, tree, "float_invert", NULL, NULL, MXML_DESCEND);
    float_invert = atoi(mxmlGetText(node, 0));
    node = mxmlFindElement(tree, tree, "aioffset", NULL, NULL, MXML_DESCEND);
    aioffset = atoi(mxmlGetText(node, 0));
    aioffset = ~aioffset + 1;
    node = mxmlFindElement(tree, tree, "aooffset", NULL, NULL, MXML_DESCEND);
    aooffset = atoi(mxmlGetText(node, 0));
    aooffset = ~aooffset + 1;
    node = mxmlFindElement(tree, tree, "dioffset", NULL, NULL, MXML_DESCEND);
    dioffset = atoi(mxmlGetText(node, 0));
    dioffset = ~dioffset + 1;
    node = mxmlFindElement(tree, tree, "dooffset", NULL, NULL, MXML_DESCEND);
    dooffset = atoi(mxmlGetText(node, 0));
    dooffset = ~dooffset + 1;

    int i = 0;
    //printf(" %lu ", sizeof(xml_map_key));
    for (node = mxmlFindElement(tree, tree,
                                "key",
                                NULL, NULL,
                                MXML_DESCEND);
         node != NULL;
         node = mxmlFindElement(node, tree,
                                "key",
                                NULL, NULL,
                                MXML_DESCEND))
    {
        xns.nodes[i] = node;
        i++;
    }
    xns.size = i;
    xmks.size = xns.size;
    xmks.xmks = calloc(xmks.size, sizeof(xml_map_key));

    for (i = 0; i < xns.size; i++)
    {
        xml_map_key *xmk = malloc(sizeof(xml_map_key));
        const char *key = mxmlElementGetAttr(xns.nodes[i], "key");
        sprintf(xmk->key, "%s", key);
        sscanf(key, "%4s%1s%2s", xmk->dev, xmk->type, xmk->number);
        xmk->slave = atoi(mxmlElementGetAttr(xns.nodes[i], "slavenumber"));
        xmk->point = atoi(mxmlElementGetAttr(xns.nodes[i], "pointnumber"));
        xmk->pointType[0] = key[4];
        printf(" %c ", xmk->pointType[0]);
        switch (xmk->pointType[0])
        {
        case '0':
            xmk->fun = ai_map_reg;
            break;
        case '1':
            xmk->fun = ao_map_reg;
            break;
        case '2':
            xmk->fun = av_map_reg;
            break;
        case '3':
            xmk->fun = bv_map_reg;
            break;
        case '4':
            xmk->fun = bv_map_reg;
            break;
        case '5':
            xmk->fun = bv_map_reg;
            break;
        default:
            break;
        }
        xmks.xmks[i] = xmk;
        printf(" %s ", key);
    }
    fclose(fp);
    // clock_t start, finish;
    //     start = clock();
    //     finish = clock();
    //     printf(" %lu ", finish - start);
}

//------------------------工具方法------------------------------
char getTypeNumberByDeviceTypeNode(mxml_node_t *deviceType)
{
    const char *deviceTypeName = deviceType->value.opaque;
    if (strcmp(deviceTypeName, "AI") == 0)
    {
        return '0';
    }
    else if (strcmp(deviceTypeName, "AV") == 0)
    {
        return '2';
    }
    else if (strcmp(deviceTypeName, "BI") == 0)
    {
        return '3';
    }
    else if (strcmp(deviceTypeName, "BO") == 0)
    {
        return '4';
    }
    else
    {
        return '\0';
    }
}