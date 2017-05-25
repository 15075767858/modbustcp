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
        printf("%s %d\n", name, where);

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