#ifndef SERVER_DISCOVER_NodeConnect_H
#define SERVER_DISCOVER_NodeConnect_H

#include "commondef.h"
#include <event2/event.h>
#include <unordered_map>
#include <event2/bufferevent.h>
#include <functional>
#include <string>

class NodeConnect {
public:
    NodeConnect(evutil_socket_t sock);
    void RegCmd();
    void SetBase(struct event_base *pBase);
    void HandlerConnect();

    void SetNodeName(std::string strName);
    std::string GetNodeName();

    static void ReadCallBack(bufferevent *bev, void *arg);
    static void WriteCallBack(bufferevent *bev, void *arg);
    static void ErrorCallBack(bufferevent *bev, short what, void *arg);

    evutil_socket_t m_sock = 0;
private:
    struct event_base *m_pBase = nullptr;
    std::string m_strNodeName;
    std::unordered_map<u16, std::function<void(u8*, int)>> m_hashMapCmd;
};

#endif //SERVER_DISCOVER_NodeConnect_H
