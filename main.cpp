#include <iostream>
#include <string>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include "globaddl.h"
#include "common/utils.h"
#include "core/threadPool.h"
#include "mainLog.h"
#include "itask.h"
#include "nodeConnect.h"
#include "httpReq.h"

namespace {
    const std::string PROFILE_PATH = "profile.json";
}

//新的客户端TCP连接过来
void ListenCallBack(evconnlistener *evListen, evutil_socket_t socket, sockaddr *sinAddr, int sinAddrLen, void *arg)
{
    log_debug("listenCallBack new connet node enter");
    NodeConnect *pNodeCon = new NodeConnect(socket);

    XThreadPool::GetInstance()->DispatchThread(pNodeCon);
}

int main() {
    //将SIGPIPE信号交给系统屏蔽处理。
    signal(SIGPIPE, SIG_IGN);

    MainLog mainlog;
    mainlog.init();
    log_info("Server Discovery start");

    XThreadPool::GetInstance()->Init(5);

    //TCP连接服务
    struct sockaddr_in sinAddr;
    bzero(&sinAddr, sizeof(sinAddr));
    sinAddr.sin_family = AF_INET;
    sinAddr.sin_port = htons(8001);

    event_base *base = event_base_new();
    if(NULL == base)
    {
        log_error("base init false");
        return -1;
    }

    evconnlistener *evListen = evconnlistener_new_bind(
                base,
                ListenCallBack,
                base,   //回调函数的参数arg
                LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
                10,    //listen block
                (sockaddr*)&sinAddr,
                sizeof(sinAddr)
            );


    //http服务
    struct evhttp* pEvhttp = evhttp_new(base);
    if(NULL == pEvhttp)
    {
        log_error("evhttp init false");
        return -2;
    }

    evhttp_bind_socket(pEvhttp, "0.0.0.0", 8888);
    evhttp_set_gencb(pEvhttp, HttpRequestCallBack, NULL);
    evhttp_set_cb(pEvhttp, "/v1/nodeManager/GetAllNodes", HttpReqGetAllNodesCb, NULL);
    evhttp_set_cb(pEvhttp, "/v1/nodeManager/StopNode", HttpReqStopNodeCb, NULL);

    event_base_dispatch(base);
    evconnlistener_free(evListen);
    event_base_free(base);

    XThreadPool::GetInstance()->UnInit();

    return 0;
}
