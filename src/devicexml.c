#include "devicexml.h"
#define XMLFILENAME "modbusID.xml"
int initDeviceXml()
{
    //创建一个xml文件
    mxml_node_t *xml = mxmlNewXML("1.0");
    //新增一个节点root
    mxml_node_t *root = mxmlNewElement(xml, "root");
    //在html节点下新增一个device节点
    mxml_node_t *device = mxmlNewElement(root, "device");
    //在device节点下新增一个AI AV BI BO节点
    mxml_node_t *AI = mxmlNewElement(device, "AI");
    mxml_node_t *key = mxmlNewElement(AI, "key");
    //key节点添加内容"Docment"
    mxmlNewText(key, 0, "UI1");
    //给key节点增加属性number，属性内容为 1001001
    mxmlElementSetAttr(key, "number", "1001001");
    mxml_node_t *AV = mxmlNewElement(device, "AV");
    mxml_node_t *BI = mxmlNewElement(device, "BI");
    mxml_node_t *BO = mxmlNewElement(device, "BO");
    //打开文件index.xml
    FILE *fp = fopen(XMLFILENAME, "wb");
    //保存节点到xml文件
    mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
    fclose(fp);
    //删除节点内存
    mxmlDelete(xml);
    return 0;
}
