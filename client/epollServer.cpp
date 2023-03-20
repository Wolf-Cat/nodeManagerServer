#include <iostream>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <thread>
#include <errno.h>
#include <sys/time.h>
#include "message.pb.h"
#include "def.h"
#include "commondef.h"
#include "pbCodec.h"
#include "dataProtocolHead.h"
#include "mainLog.h"
#include "handleEvent.h"
using namespace std;

extern int g_sockSerd;

void sig_alrm_hander(int sig_num)
{
    if(sig_num == SIGALRM)
    {
        std::cout << "sig_alrm_hander SIGALRM" << std::endl;
        u8 buffer[1024]={0};

        serdmessage::ReqHeartBeat req;
        int nBodyLen = req.ByteSizeLong();

        StructProtocolHead head;
        head.usTag = 100;
        head.ucVersion = 2;
        head.ucType = 1;
        head.usLen = nBodyLen;
        head.usMsgId = REQ_HEART_BEAT;
        head.ullMsgSn = 100;
        head.uiPriority = 456;
        head.uiReserver1 = 789;

        DataHeadCodec headCodec;
        headCodec.Encode(&head, buffer, sizeof(buffer));
        //注意：不能采用sizeof(head) 这样的方式，会有结构体字节的对齐问题导致大小不是预想的那样
        req.SerializeToArray(buffer + BODY_MSG_POS, nBodyLen);

        send(g_sockSerd, buffer, sizeof(buffer), 0);
    }
}

int main()
{
    signal(SIGALRM, sig_alrm_hander);
    //MainLog log;
    //log.init();

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);  // 连接到服务发现的套接字
    g_sockSerd = sockfd;
    struct sockaddr_in serd_addr;
    memset(&serd_addr, 0, sizeof(serd_addr));
    serd_addr.sin_family = AF_INET;
    serd_addr.sin_port = htons(8001);
    inet_pton(AF_INET, "127.0.0.1", &serd_addr.sin_addr);

    int n = connect(sockfd, (struct sockaddr*)&serd_addr, sizeof(serd_addr));

    //自己本身的监听套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // 监听本机所有Ip
    serv_addr.sin_port = htons(9999);

    bind(lfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(lfd, 100);

    //Todo: 设置端口复用，sockopt,解决2msl问题

    int efd = epoll_create(2000);

    AddEvent(efd, lfd, EPOLLIN);
    SedFdNoBlock(lfd);

    //连接到服务发现的事件
    AddEvent(efd, sockfd, EPOLLIN | EPOLLOUT| EPOLLET);
    SedFdNoBlock(sockfd);

    //获取终端输入的事件
    AddEvent(efd, STDIN_FILENO, EPOLLIN | EPOLLET);
    SedFdNoBlock(STDIN_FILENO);

    //epoll传出参数
    struct epoll_event evOutArray[2000];

    while(1)
    {
        int nRet = epoll_wait(efd, evOutArray, sizeof(evOutArray) / sizeof(evOutArray[0]), 1000);

        for (int i = 0; i < nRet; ++i)
        {
            char databuf[1024] = {0};
            int fd = evOutArray[i].data.fd;
            if(fd == lfd && evOutArray[i].events & EPOLLIN)  //有新的连接进来
            {
                HandleAccept(efd, lfd);
            }
            else if(evOutArray[i].events & EPOLLIN)
            {
                HandleRead(efd, fd, databuf);
            }
            else if(evOutArray[i].events & EPOLLOUT)
            {
                HandleWrite(efd, fd, databuf);
            }
        }
    }
    close(lfd);
    return 0;
}
