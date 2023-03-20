#ifndef SERVER_DISCOVER_XTHREAD_H
#define SERVER_DISCOVER_XTHREAD_H

#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <list>
#include <event2/event.h>
#include "nodeConnect.h"

class Xthread {
public:
    Xthread(int nThreadId);
    ~Xthread();

    void Start();
    void Run();
    void Stop();

    bool SetUpEvent();

    //收到线程池发出的激活消息（线程池的分发）
    void Notify(evutil_socket_t fd, short which);
    void AddConnect(NodeConnect *pNode);
    void Active();

private:
    int m_nId = 0;
    struct event_base* m_base = nullptr;
    std::shared_ptr<std::thread> m_pthread;
    int m_fdPipeWirte = 0;   //管道的写端

    std::mutex m_mutex;

    std::list<NodeConnect*> m_listconnect;
};

#endif //SERVER_DISCOVER_XTHREAD_H
