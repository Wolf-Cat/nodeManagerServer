#ifndef SERVER_DISCOVER_DATAStructProtocolHead_H
#define SERVER_DISCOVER_DATAStructProtocolHead_H

#include "commondef.h"

enum PROTOCOL_TYPE    //传输的数据格式
{
    JSON_PROTOCAL_TYPE = 0,   //json格式数据
    PB_PROTOCAL_TYPE = 1,     //protobuf
    BINARY_PROTOCAL_TYPE = 2  //二进制类型
};

enum MSG_ARROW       // 数据传输的方向
{
    MS_2_SD = 0,     //流媒体服务器节点--> 服务发现服务器
    SD_2_MS = 1,     //服务发现服务器--> 流媒体服务器节点

    STATIS_2_SD = 2,  //统计节点 --> 服务发现
    SD_2_STATIS = 3   // 服务发现 --> 统计节点
};

//传输的数据的消息头
struct StructProtocolHead {
    u16 usTag;        //先导码 OxABCD, 标识一条消息的开始
    u8  ucVersion;    //协议版本号
    u8  ucType;       //数据的格式 0: json, 1:protobuf
    u16 usLen;        //消息体的长度（不包含消息头）
    u16 usMsgId;      //消息的业务ID
    u64 ullMsgSn;  //(8字节)消息的唯一标识号，//最低14位表示节点的类型，接着4位为节点的序号，再6位为通信方向，最后40位为消息序列号
    u32 uiPriority;     // 消息的优先级
    u32 uiReserver1;    //预留字段1
};

//头部解析时的起始偏移位置
#define HEAD_TAG_POS       0
#define HEAD_VERSION_POS   2
#define HEAD_TYPER_POS      3
#define HEAD_LEN_POS       4
#define HEAD_MSG_ID_POS    6
#define HEAD_MSG_SN_POS    8
#define HEAD_MSG_PRIORITY     16
#define HEAD_MSG_RSV1     20

#define BODY_MSG_POS      24

class DataHeadCodec {
public:
    virtual bool Decode(u8* buffer, u32 unSize, StructProtocolHead *pHead);  //解析包头函数
    virtual bool Encode(StructProtocolHead *pHead, u8* buffer, u32 unSize);

private:
    u8 GetByte(u8* buffer, u32 unSize, u32 unOffest);
    void SetByte(u8* buffer, u32 unSize, u32 unOffest, u8 ucVal);

    u16 GetUnsignedShort(u8* buffer, u32 unSize, u32 unOffest);
    void SetUnsignedShort(u8* buffer, u32 unSize, u32 unOffest, u16 usVal);

    u32 GetUnsignedInt(u8* buffer, u32 unSize, u32 unOffest);
    void SetUnsignedInt(u8* buffer, u32 unSize, u32 unOffest, u32 unVal);

    u64 GetUnsignedLonglong(u8* buffer, u32 unSize, u32 unOffest);
    void SetUnsignedLonglong(u8* buffer, u32 unSize, u32 unOffest, u64 ullVal);
};

#endif //SERVER_DISCOVER_DATAStructProtocolHead_H
