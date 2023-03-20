#include "handleEvent.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include "message.pb.h"
#include "def.h"
#include "commondef.h"
#include "pbCodec.h"
#include "dataProtocolHead.h"
#include <iostream>

using namespace std;

struct sockaddr_in clientAddr;   //待新的客户端连接上来准备的clientAddr
socklen_t sockAddrLen = sizeof(clientAddr);
int g_sockSerd = 0;

void HandleAccept(int epollfd, int listenfd)
{
    int cfd = accept(listenfd, (struct sockaddr*)&clientAddr, &sockAddrLen);
    if(cfd == -1)
    {
        perror("连接出现错误");
        return;
    }
    else if(cfd < 0)
    {
        if(errno == EINTR)
        {
            return;
        }
    }

    SedFdNoBlock(cfd);

    struct epoll_event evClient;
    evClient.data.fd = cfd;
    evClient.events = EPOLLIN | EPOLLOUT | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, cfd, &evClient);
    char ip[64] = {0};
    printf("New client ip: %s, port: %d\n", inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, ip, sizeof(ip)),
           ntohs(clientAddr.sin_port));
}

void HandleRead(int epollfd, int fd, char* buf)
{
    int nRecLen = 0;
    if(fd == STDIN_FILENO)
    {
        while ((nRecLen = read(fd, buf, sizeof(buf)) )> 0)
        {

        }
        std::cout << "has STDIN_FILENO:" << buf << endl;
        struct itimerval tv, otv;
        tv.it_value.tv_sec = 3;
        tv.it_value.tv_usec = 0;

        tv.it_interval.tv_sec = 0;
        tv.it_interval.tv_usec = 0;
        setitimer(ITIMER_REAL, &tv, &otv);

        ModifyEvent(epollfd, g_sockSerd, EPOLLIN | EPOLLET);
        SendLoginToSerd(g_sockSerd);
    }
    else
    {
        while ((nRecLen = recv(fd, buf, sizeof(buf), 0)) > 0)
        {

        }
        //RecvLoginResp(g_sockSerd, buf);
        //ModifyEvent(epollfd, g_sockSerd, EPOLLOUT | EPOLLET);
    }

    if(nRecLen == 0)
    {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
    }
    else if(nRecLen == -1)
    {
        if(errno == EAGAIN)
        {
            printf("缓冲区数据已经读完\n");
        }
        else
        {
            printf("别的错误：%d, errno=%d\n", nRecLen, errno);
        }
        perror("recv err");
    }

        if(fd == STDIN_FILENO)
        {
            ModifyEvent(epollfd, g_sockSerd, EPOLLOUT | EPOLLET);
            std::cout << "ddddddddata:" << buf << endl;
            //SendLoginToSerd(g_sockSerd);
            //AddEvent(epollfd, g_sockSerd,  EPOLLOUT | EPOLLET);
            //ModifyEvent(epollfd, g_sockSerd, EPOLLIN | EPOLLOUT | EPOLLET);
            SendLoginToSerd(g_sockSerd);
        }
        else
        {
            //DeleteEvent(epollfd, g_sockSerd, EPOLLIN | EPOLLET);
            RecvLoginResp(g_sockSerd, buf);
            ModifyEvent(epollfd, g_sockSerd, EPOLLOUT | EPOLLET);

        }
}

void HandleWrite(int epollfd, int fd, char* buf)
{
    if(fd == g_sockSerd)
    {
        std::cout << "HandleWrite:" << endl;
        //SendLoginToSerd(g_sockSerd);
        ModifyEvent(epollfd, g_sockSerd, EPOLLIN | EPOLLET);
    }
}

void AddEvent(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void DeleteEvent(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;

    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

void ModifyEvent(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;

    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}

void SedFdNoBlock(int fd)
{
    int flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
}

void SendLoginToSerd(int fd)
{
    u8 buffer[1024]={0};

    serdmessage::ReqNodeLogin reqLogin;
    reqLogin.set_city("guangzhou");
    reqLogin.set_ip("123.456.789");
    reqLogin.set_port(105);
    int nBodyLen = reqLogin.ByteSizeLong();

    StructProtocolHead head;
    head.usTag = 100;
    head.ucVersion = 2;
    head.ucType = 1;
    head.usLen = nBodyLen;
    head.usMsgId = REQ_NODE_LOGIN;
    head.ullMsgSn = 100;
    head.uiPriority = 456;
    head.uiReserver1 = 789;

    DataHeadCodec headCodec;
    headCodec.Encode(&head, buffer, sizeof(buffer));
    //注意：不能采用sizeof(head) 这样的方式，会有结构体字节的对齐问题导致大小不是预想的那样
    reqLogin.SerializeToArray(buffer + BODY_MSG_POS, nBodyLen);
    send(g_sockSerd, buffer, sizeof(buffer), 0);
}

void RecvLoginResp(int fd, char *buf)
{
    std::cout << "RecvLoginResp: " << endl;
    StructProtocolHead head;

    DataHeadCodec dec;
    dec.Decode((u8*)buf, sizeof(buf), &head);

    if(head.ucType != 1)
    {
        return;
    }

    PbCodec pbMsg;
    //注意：不能采用 buff + sizeof(head) 这样的方式，会有结构体字节的对齐问题导致大小不是预想的那样. 只能用偏移
    pbMsg.Decode(head.usMsgId, (u8*)buf + BODY_MSG_POS, head.usLen);

    std::cout<< "get tag:" << head.usTag << std::endl;
    printf("version :%d\n", head.ucVersion);
    printf("type :%d\n", head.ucType);
    std::cout<< "ucVersion:" << head.ucVersion << std::endl;
    std::cout<< "head2:" << head.usLen << std::endl;
    std::cout<< "msgId:" << head.usMsgId << std::endl;
    std::cout<< "ullMsgSn:" << head.ullMsgSn << std::endl;
    std::cout<< "uiPriority:" << head.uiPriority << std::endl;
    std::cout<< "uiReserver1:" << head.uiReserver1 << std::endl;
}