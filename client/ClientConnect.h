#ifndef SERVER_DISCOVER_CLIENTCONNECT_H
#define SERVER_DISCOVER_CLIENTCONNECT_H

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <iostream>

using namespace std;

class ClientConnect {
public:
    ClientConnect(struct event_base *base);
    ~ClientConnect();

    bool ConnectToSerd();

    static void ReadCB(struct bufferevent *bev, void *arg);
    static void WriteCB(struct bufferevent *bev, void *arg);
    static void ErrorCB(struct bufferevent *bev, short event, void *arg);

    static void HeartTimeoutCB(int fd, short what, void *arg);

    void UpdateHeartTime();
    uint32_t GetTimeHeartTime();
    struct event_base* GetBase();
    struct bufferevent* GetBev();
    struct event* GetTimer();
    void SetEvTimer(struct event* ev);
    void ReqLogin();
    void ReqHeatBeat();
    bool IsConnected();
    void SetConnectState(bool bConnect);

private:
    uint32_t m_timeLastHeart = 0;
    int m_sockFd;
    struct bufferevent* m_pBev;
    struct event_base* m_pBase;
    struct event* m_pevTimer;
    bool m_bConnected = false;
};

#endif //SERVER_DISCOVER_CLIENTCONNECT_H
