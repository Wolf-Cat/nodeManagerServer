#include "pbCodec.h"
#include "mainLog.h"
#include "message.pb.h"
#include "commondef.h"
#include "dataProtocolHead.h"
#include <iostream>
#include <sstream>

void PbCodec::Decode(u16 msgId, u8 *buffer, u32 dataLen)
{
    switch (msgId) {
        case REQ_NODE_LOGIN:
            HandleNodeOnlineReq(buffer, dataLen);
            break;
        case RESP_NODE_LOGIN:
            HandleNodeOnlineResp(buffer, dataLen);
            log_info("RESP_NODE_LOGIN Decode");
            break;
        case REQ_HEART_BEAT:
            log_debug("REQ_HEART_BEAT Decode");
            break;
        case RESP_HEART_BEAT:
            break;
        default:
            log_error("msg id = %u is invalid", msgId);
            break;
    }
}

bool PbCodec::Encode(MsgID cid, u8 *buffer, u32 size)
{
    return true;
}

NodeInfo PbCodec::HandleNodeOnlineReq(u8* buffer, u32 dataLen)
{
    serdmessage::ReqNodeLogin req;
    std::istringstream ss;
    ss.rdbuf()->pubsetbuf((char *)buffer, dataLen);
    req.ParseFromIstream(&ss);

    NodeInfo infoRet;
    infoRet.strIp = req.ip();
    infoRet.strName = req.name();
    infoRet.strCity = req.city();

    std::cout << "protodata ip: " << infoRet.strIp << std::endl;
    std::cout << "protodata port: " << req.port() << std::endl;
    std::cout << "protodata city :" << infoRet.strCity  << std::endl;
    std::cout << "protodata name :" << infoRet.strName << std::endl;

    return infoRet;
}

void PbCodec::HandleNodeOnlineResp(u8* buffer, u32 dataLen)
{
    serdmessage::RespNodeLogin resp;
    std::istringstream ss;
    ss.rdbuf()->pubsetbuf((char *)buffer, dataLen);
    resp.ParseFromIstream(&ss);

    log_debug("protodata HandleNodeOnlineResp: %u", resp.ret_code());
}

