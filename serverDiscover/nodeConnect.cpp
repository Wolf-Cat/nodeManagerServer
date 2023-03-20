#include "nodeConnect.h"
#include <iostream>
#include "mainLog.h"
#include "dataProtocolHead.h"
#include "pbCodec.h"
#include "message.pb.h"
#include "global.h"
#include <stdio.h>

NodeConnect::NodeConnect(evutil_socket_t sock) {
    m_sock = sock;
}

void NodeConnect::SetBase(struct event_base *pBase) {
    m_pBase = pBase;
}

void NodeConnect::ReadCallBack(struct bufferevent *bev, void *arg)
{
    NodeConnect *pThis = (NodeConnect*)arg;
    if(pThis == nullptr)
    {
        return;
    }

    u8 databuff[1024] = {0};

    int len = bufferevent_read(bev, databuff, sizeof(databuff));
    if(len <= 0)
    {
        return;
    }

    StructProtocolHead head;
    DataHeadCodec dec;
    dec.Decode(databuff, sizeof(databuff), &head);

    if(head.ucType != PB_PROTOCAL_TYPE) {
        return;
    }

    PbCodec pbMsg;
    //注意：不能采用 buff + sizeof(head) 这样的方式，会有结构体字节的对齐问题导致大小不是预想的那样. 只能用偏移
    pbMsg.Decode(head.usMsgId, databuff + BODY_MSG_POS, head.usLen);

    u8 bufferSend[1024] = {0};
    StructProtocolHead headSend;
    headSend.usTag = 100;
    headSend.ucVersion = 2;
    headSend.ucType = PB_PROTOCAL_TYPE;
    headSend.ullMsgSn = 101;
    headSend.uiPriority = 789;
    headSend.uiReserver1 = 456;

    if(head.usMsgId == REQ_HEART_BEAT)
    {
        serdmessage::RespHeartBeat resp;
        int nBodyDataLen = resp.ByteSizeLong();

        headSend.usLen = nBodyDataLen;
        headSend.usMsgId = RESP_HEART_BEAT;

        DataHeadCodec headCodec;
        headCodec.Encode(&headSend, bufferSend, sizeof(bufferSend));
        resp.SerializeToArray(bufferSend + BODY_MSG_POS, nBodyDataLen);
    }
    else if(head.usMsgId == REQ_NODE_LOGIN)
    {
        NodeInfo info = pbMsg.HandleNodeOnlineReq(databuff + BODY_MSG_POS, head.usLen);
        pThis->SetNodeName(info.strName);
        std::cout << "inf: " << info.strName << std::endl;

        Global::GetInstance()->AddNode(info.strIp, pThis);
        serdmessage::RespNodeLogin resp;
        resp.set_ret_code(6789);
        int nBodyDataLen = resp.ByteSizeLong();

        headSend.usLen = nBodyDataLen;
        headSend.usMsgId = RESP_NODE_LOGIN;

        DataHeadCodec headCodec;
        headCodec.Encode(&headSend, bufferSend, sizeof(bufferSend));
        resp.SerializeToArray(bufferSend + BODY_MSG_POS, nBodyDataLen);
    }
    bufferevent_write(bev, bufferSend, sizeof(bufferSend));
}

void NodeConnect::WriteCallBack(bufferevent *bev, void *arg)
{
    log_debug("NodeConnect::WriteCallBack---");
}

void NodeConnect::ErrorCallBack(bufferevent *bev, short what, void *arg)
{
    if(what & BEV_EVENT_ERROR)
    {
        log_debug("BEV_EVENT_ERROR");
        NodeConnect *pThis = (NodeConnect *)arg;
        delete pThis;
    }
    else if(what & BEV_EVENT_TIMEOUT)
    {
        log_debug("BEV_EVENT_TIMEOUT");
        NodeConnect *pThis = (NodeConnect *)arg;
        delete pThis;
    }
    else if(what & BEV_EVENT_EOF)
    {
        log_debug("BEV_EVENT_EOF");
        NodeConnect *pThis = (NodeConnect *)arg;
        delete pThis;
    }
}

void NodeConnect::RegCmd()
{

}

void NodeConnect::HandlerConnect() {
    log_debug("NodeConnect HandlerTask build New Connect");
    bufferevent *bev = bufferevent_socket_new(m_pBase, m_sock, BEV_OPT_CLOSE_ON_FREE);
    if(NULL == bev)
    {
        delete this;
        return;
    }

    bufferevent_setcb(bev, ReadCallBack, WriteCallBack, ErrorCallBack, this);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

void NodeConnect::SetNodeName(std::string strName)
{
    m_strNodeName = strName;
}

std::string NodeConnect::GetNodeName()
{
    return m_strNodeName;
}