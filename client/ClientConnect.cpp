#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include "ClientConnect.h"
#include "message.pb.h"
#include "def.h"
#include "commondef.h"
#include "pbCodec.h"
#include "dataProtocolHead.h"
#include "mainLog.h"
#include "utils.h"
#include "ClientConnect.h"

namespace {
    const int HEART_BEAT_TIME_PASS_INTERVAL = 15;
}

ClientConnect::ClientConnect(struct event_base *base)
{
    m_pBase = base;
}

ClientConnect::~ClientConnect()
{

}

bool ClientConnect::ConnectToSerd()
{
    struct sockaddr_in sinAddr;
    memset(&sinAddr, 0, sizeof(sinAddr));
    sinAddr.sin_family = AF_INET;
    sinAddr.sin_port = htons(8001);
    inet_pton(AF_INET, "127.0.0.1", &sinAddr.sin_addr.s_addr);

    m_sockFd = socket(AF_INET, SOCK_STREAM, 0);

    m_pBev = bufferevent_socket_new(m_pBase, m_sockFd, BEV_OPT_CLOSE_ON_FREE);
    // 注意bufferevent_socket_connect()函数是非阻塞的，调用后会马上返回。
    // 返回0不代表和服务器连接成功，返回-1一定是连接失败
    int nRetConnect = bufferevent_socket_connect(m_pBev, (struct sockaddr*)&sinAddr, sizeof(sinAddr));
    if(m_pBev == NULL || nRetConnect == -1)
    {
        return false;
    }

    bufferevent_enable(m_pBev, EV_READ | EV_WRITE);
    bufferevent_setcb(m_pBev, ReadCB, WriteCB, ErrorCB, this);

    return true;
}

void ClientConnect::ReadCB(struct bufferevent *bev, void *arg) {
    u8 databuff[1024] = {0};

    int len = bufferevent_read(bev, databuff, sizeof(databuff));
    if(len <= 0)
    {
        return;
    }

    StructProtocolHead head;

    DataHeadCodec dec;
    dec.Decode(databuff, sizeof(databuff), &head);

    if(head.ucType != PB_PROTOCAL_TYPE)
    {
        return;
    }

    PbCodec pbMsg;
    //注意：不能采用 buff + sizeof(head) 这样的方式，会有结构体字节的对齐问题导致大小不是预想的那样. 只能用偏移
    pbMsg.Decode(head.usMsgId, databuff + BODY_MSG_POS, head.usLen);

    ClientConnect *pthis = (ClientConnect *)arg;
    if(pthis == nullptr)
    {
        return;
    }

    if(head.usMsgId == RESP_NODE_LOGIN)
    {
        log_info("RESP_NODE_LOGIN get, start heart");
        pthis->UpdateHeartTime();

        struct event *pEvTimer = event_new(pthis->GetBase(), -1, EV_TIMEOUT | EV_PERSIST, HeartTimeoutCB, pthis);
        pthis->SetEvTimer(pEvTimer);
        struct timeval timeHeart = {5, 0};
        event_add(pthis->GetTimer(), &timeHeart);
    }
    else if(head.usMsgId == RESP_HEART_BEAT)
    {
        pthis->UpdateHeartTime();
        log_info("heart beat resp get, tcp connect normal");
    }
    else if(head.usMsgId == REQ_CLOSE_NODE)
    {
        log_info("Get server close node req , will close node");
        exit(0);
    }
}

void ClientConnect::HeartTimeoutCB(int fd, short what, void *arg)
{
    ClientConnect *pthis = (ClientConnect *)arg;
    if(pthis == nullptr)
    {
        return;
    }

    uint32_t timeCur = Utils::GetCurTimeStamp();
    if(timeCur - pthis->GetTimeHeartTime() > HEART_BEAT_TIME_PASS_INTERVAL)   //超时重连
    {
        log_error("serd disconnect, try to reconnect");
        if(pthis->ConnectToSerd() && pthis->IsConnected())
        {
            log_info("reconnect to discovery server success");
            pthis->UpdateHeartTime();
            pthis->ReqLogin();       //重连后重新登录
        }
        else
        {
            log_error("reconnect to discovery server failed");
            return;
        }
    }

    pthis->ReqHeatBeat();
}

void ClientConnect::UpdateHeartTime()
{
    m_timeLastHeart = Utils::GetCurTimeStamp();
}

uint32_t ClientConnect::GetTimeHeartTime()
{
    return m_timeLastHeart;
}

struct event_base* ClientConnect::GetBase()
{
    return m_pBase;
}

struct bufferevent* ClientConnect::GetBev()
{
    return m_pBev;
}

struct event* ClientConnect::GetTimer()
{
    return m_pevTimer;
}

void ClientConnect::SetEvTimer(struct event* ev)
{
    m_pevTimer = ev;
}

void ClientConnect::ReqLogin()
{
    u8 buffer[1024] = {0};
    serdmessage::ReqNodeLogin reqLogin;
    reqLogin.set_city("guangzhou");
    reqLogin.set_ip("123.456.789.452");
    reqLogin.set_port(105);
    reqLogin.set_name("node_ms_01");
    int nBodyLen = reqLogin.ByteSizeLong();

    StructProtocolHead head;
    head.usTag = 100;
    head.ucVersion = 2;
    head.ucType = PB_PROTOCAL_TYPE;
    head.usLen = nBodyLen;
    head.usMsgId = REQ_NODE_LOGIN;
    head.ullMsgSn = 100;
    head.uiPriority = 456;
    head.uiReserver1 = 789;

    DataHeadCodec headCodec;
    headCodec.Encode(&head, buffer, sizeof(buffer));
    reqLogin.SerializeToArray(buffer + BODY_MSG_POS, nBodyLen);
    bufferevent_write(m_pBev, buffer, sizeof(buffer));
}

void ClientConnect::ReqHeatBeat()
{
    serdmessage::ReqHeartBeat reqHeat;
    int nBodyLen = reqHeat.ByteSizeLong();

    u8 buffer[1024] = {0};
    StructProtocolHead head;
    head.usTag = 100;
    head.ucVersion = 2;
    head.ucType = PB_PROTOCAL_TYPE;
    head.usLen = nBodyLen;
    head.usMsgId = REQ_HEART_BEAT;
    head.ullMsgSn = 100;
    head.uiPriority = 4789;
    head.uiReserver1 = 54;

    DataHeadCodec headCodec;
    headCodec.Encode(&head, buffer, sizeof(buffer));
    //注意：不能采用sizeof(head) 这样的方式，会有结构体字节的对齐问题导致大小不是预想的那样
    reqHeat.SerializeToArray(buffer + BODY_MSG_POS, nBodyLen);
    bufferevent_write(m_pBev, buffer, sizeof(buffer));
}
void ClientConnect::WriteCB(struct bufferevent *bev, void *arg)
{
    log_debug("client WriteCB---");
}

void ClientConnect::ErrorCB(struct bufferevent *bev, short event, void *arg)
{
    ClientConnect *pthis = (ClientConnect *)arg;
    if(pthis == nullptr)
    {
        return;
    }

    if(event & BEV_EVENT_EOF)
    {
        //网络连接正常状态下，若服务器关闭，则BEV_EVENT_EOF事件触发
        pthis->SetConnectState(false);
        log_error("Connection serd  process closed");
    }
    else if(event & BEV_EVENT_ERROR)
    {
        pthis->SetConnectState(false);
        log_error("connect error");
    }
    else if(event & BEV_EVENT_CONNECTED)
    {
        pthis->SetConnectState(true);
        log_info("已经成功连接服务器..");
    }
}

bool ClientConnect::IsConnected()
{
    return m_bConnected;
}

void ClientConnect::SetConnectState(bool bConnect)
{
    m_bConnected = bConnect;
}