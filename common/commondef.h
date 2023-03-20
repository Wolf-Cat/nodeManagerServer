#ifndef SERVER_DISCOVER_COMMONDEF_H
#define SERVER_DISCOVER_COMMONDEF_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int  u32;     /* int == long */
typedef unsigned long long u64;

enum MsgID {
    REQ_HEART_BEAT = 100,
    REQ_NODE_LOGIN = 101,
    REQ_CLOSE_NODE = 102,

    RESP_HEART_BEAT = 1000,
    RESP_NODE_LOGIN = 1001,
};

#endif //SERVER_DISCOVER_COMMONDEF_H
