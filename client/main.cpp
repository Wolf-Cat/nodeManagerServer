//客户端程序，连接到服务端

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <event2/event.h>
#include <string.h>
#include <stdio.h>
#include <thread>
#include <chrono>
#include "message.pb.h"
#include "commondef.h"
#include "dataProtocolHead.h"
#include "utils.h"
#include "def.h"

using namespace std;

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in s_addr;
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(8001);

    inet_pton(AF_INET, "127.0.0.1", &s_addr.sin_addr);

    int n = connect(sockfd, (struct sockaddr*)&s_addr, sizeof(s_addr));
    std::cout << "errconne = " << n <<endl;
    perror("coonect errr:");

    u8 buffer[1024]={0};

    StructProtocolHead head;
    head.usTag = htons(100);
    head.ucVersion = 2;
    head.ucType = 1;
    head.usLen = 0;
    head.usMsgId = htons(REQ_NODE_LOGIN);
    head.ullMsgSn = 100;
    head.uiPriority = htonl(456);
    head.uiReserver1 = htonl(789);

    std::cout << head.usTag << " dd" << head.ucVersion << "  ddd " << std::endl;
    printf("mmmversion:%d\n", head.ucVersion);

    serdmessage::ReqNodeLogin reqLogin;
    reqLogin.set_city("guangzhou");
    reqLogin.set_ip("123.456.789");
    reqLogin.set_port(105);
    int nBodyLen = reqLogin.ByteSizeLong();
    printf("nBody = %d\n", nBodyLen);

    head.usLen = htons(nBodyLen);

    memcpy(buffer + HEAD_TAG_POS, &head.usTag, sizeof(u16));
    memcpy(buffer + HEAD_VERSION_POS, &head.ucVersion, sizeof(u8));
    memcpy(buffer + HEAD_TYPER_POS, &head.ucType, sizeof(u8));
    memcpy(buffer + HEAD_LEN_POS, &head.usLen, sizeof(u16));
    memcpy(buffer + HEAD_MSG_ID_POS, &head.usMsgId, sizeof(u16));
    memcpy(buffer + HEAD_MSG_SN_POS, &head.ullMsgSn, sizeof(u64));
    memcpy(buffer + HEAD_MSG_PRIORITY, &head.uiPriority, sizeof(u32));
    memcpy(buffer + HEAD_MSG_RSV1, &head.uiReserver1, sizeof(u32));

    //注意：不能采用sizeof(head) 这样的方式，会有结构体字节的对齐问题导致大小不是预想的那样
    reqLogin.SerializeToArray(buffer + BODY_MSG_POS, nBodyLen);

    std::cout << head.usTag << " dd" << head.ucVersion << "  ddd "<< buffer[3] << std::endl;

    send(sockfd, buffer, sizeof(buffer), 0);


    //发送登录请求成功后收到resp，就开始打开定时器
    while(1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        u32 curTime = Utils::GetCurTimeStamp();
        std::cout << "curTime:" << curTime << std::endl;
    }

    close(sockfd);

    return 0;
}