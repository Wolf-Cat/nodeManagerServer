#ifndef SERVER_DISCOVER_PBCODEC_H
#define SERVER_DISCOVER_PBCODEC_H

#include "commondef.h"
#include <iostream>

struct NodeInfo {
    std::string strIp;
    int port;
    std::string strCity;
    std::string strName;
};

class PbCodec {
public:
    void Decode(u16 msgId, u8 *buffer, u32 dataLen);
    bool Encode(MsgID cid, u8 *buffer, u32 size);
    NodeInfo HandleNodeOnlineReq(u8* buffer, u32 dataLen);
    void HandleNodeOnlineResp(u8* buffer, u32 dataLen);
};


#endif //SERVER_DISCOVER_PBCODEC_H
