#include "dataProtocolHead.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>

bool DataHeadCodec::Decode(u8 *buffer, u32 unSize, StructProtocolHead *pHead) {
    assert(buffer != NULL);

    pHead->usTag = GetUnsignedShort(buffer, unSize, HEAD_TAG_POS);
    pHead->ucVersion = GetByte(buffer, unSize, HEAD_VERSION_POS);
    pHead->ucType = GetByte(buffer, unSize, HEAD_TYPER_POS);
    pHead->usLen = GetUnsignedShort(buffer, unSize, HEAD_LEN_POS);
    pHead->usMsgId = GetUnsignedShort(buffer, unSize, HEAD_MSG_ID_POS);
    pHead->ullMsgSn = GetUnsignedLonglong(buffer, unSize, HEAD_MSG_SN_POS);
    pHead->uiPriority = GetUnsignedInt(buffer, unSize, HEAD_MSG_PRIORITY);
    pHead->uiReserver1 = GetUnsignedInt(buffer, unSize, HEAD_MSG_RSV1);

    return true;
}

bool DataHeadCodec::Encode(StructProtocolHead *pHead, u8 *buffer, u32 unSize) {
    assert(buffer != NULL && pHead != nullptr);

    SetUnsignedShort(buffer, unSize, HEAD_TAG_POS, pHead->usTag);
    SetByte(buffer, unSize, HEAD_VERSION_POS, pHead->ucVersion);
    SetByte(buffer, unSize, HEAD_TYPER_POS, pHead->ucType);
    SetUnsignedShort(buffer, unSize, HEAD_LEN_POS, pHead->usLen);
    SetUnsignedShort(buffer, unSize, HEAD_MSG_ID_POS, pHead->usMsgId);
    SetUnsignedLonglong(buffer, unSize, HEAD_MSG_SN_POS, pHead->ullMsgSn);
    SetUnsignedInt(buffer, unSize, HEAD_MSG_PRIORITY, pHead->uiPriority);
    SetUnsignedInt(buffer, unSize, HEAD_MSG_RSV1, pHead->uiReserver1);

    return true;
}

u8 DataHeadCodec::GetByte(u8 *buffer, u32 unSize, u32 unOffest) {
    assert(buffer != NULL);
    assert(unOffest <= unSize);

    return *(buffer + unOffest);
}

u16 DataHeadCodec::GetUnsignedShort(u8 *buffer, u32 unSize, u32 unOffest) {
    assert(buffer != NULL);
    assert(unOffest + sizeof(u16) <= unSize);

    u16 val = 0;
    memcpy(&val, buffer + unOffest, sizeof(u16));   //两字节就需要考虑大小端的问题了，一个字节不用考虑
    return ntohs(val);
}

u32 DataHeadCodec::GetUnsignedInt(u8 *buffer, u32 unSize, u32 unOffest) {
    assert(buffer != NULL);
    //assert(unOffest + sizeof(u32) <= unSize);

    u32 val = 0;
    memcpy(&val, buffer + unOffest, sizeof(u32));
    return ntohs(val);
}

u64 DataHeadCodec::GetUnsignedLonglong(u8 *buffer, u32 unSize, u32 unOffest) {
    assert(buffer != NULL);
    //assert(unOffest + sizeof(u64) <= unSize);

    u64 val = GetUnsignedInt(buffer, unSize, unOffest);
    val = val << 32;
    val |= GetUnsignedInt(buffer, unSize, unOffest + sizeof(u32));
    return val;
}


void DataHeadCodec::SetByte(u8 *buffer, u32 unSize, u32 unOffest, u8 ucVal) {
    assert(buffer != NULL);
    assert(unOffest + sizeof(u8) <= unSize);

    buffer[unOffest] = ucVal;
}

void DataHeadCodec::SetUnsignedShort(u8 *buffer, u32 unSize, u32 unOffest, u16 usVal) {
    assert(buffer != NULL);
    assert(unOffest + sizeof(u8) <= unSize);

    u16 temp = htons(usVal);
    memcpy(buffer + unOffest, &temp, sizeof(u16));
}

void DataHeadCodec::SetUnsignedInt(u8* buffer, u32 unSize, u32 unOffest, u32 unVal)
{
    assert(buffer != NULL);
    assert(unOffest + sizeof(u8) <= unSize);

    u32 temp = htons(unVal);
    memcpy(buffer + unOffest, &temp, sizeof(u32));
}

void DataHeadCodec::SetUnsignedLonglong(u8 *buffer, u32 unSize, u32 unOffest, u64 ullVal) {
    assert(buffer != NULL);
    assert(unOffest + sizeof(u8) <= unSize);

    u32 temp = htonl(static_cast<u32>(ullVal));
    memcpy(buffer + unOffest + sizeof(u32), &temp, sizeof(u32));

    ullVal = ullVal >> 32;
    temp = htonl(static_cast<u32> (ullVal));
    memcpy(buffer + unOffest, &ullVal, sizeof(ullVal));
}