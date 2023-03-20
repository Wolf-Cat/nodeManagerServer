#ifndef SERVER_DISCOVER_JSONCODEC_H
#define SERVER_DISCOVER_JSONCODEC_H

#include "commondef.h"

class JsonCodec {
    bool Encode(MsgID cid, u8 *buffer, u32 size);
    void decode(u16 msgId, u8 *buffer, u32 size);
};

#endif //SERVER_DISCOVER_JSONCODEC_H
