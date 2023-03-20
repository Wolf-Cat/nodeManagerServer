#ifndef SERVER_DISCOVER_NODELOGINREQ_H
#define SERVER_DISCOVER_NODELOGINREQ_H

#include "itask.h"

class NodeLoginReq : public Itask {
public:
    NodeLoginReq(evutil_socket_t sock) : Itask(sock) {}

    virtual void HandlerTask() override;
};

#endif //SERVER_DISCOVER_NODELOGINREQ_H
