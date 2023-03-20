#ifndef SERVER_DISCOVER_DEF_H
#define SERVER_DISCOVER_DEF_H

//头部解析时的起始偏移位置
#define HEAD_TAG_POS       0
#define HEAD_VERSION_POS   2
#define HEAD_TYPER_POS      3
#define HEAD_LEN_POS       4
#define HEAD_MSG_ID_POS    6
#define HEAD_MSG_SN_POS    8
#define HEAD_MSG_PRIORITY     16
#define HEAD_MSG_RSV1     20

#define BODY_MSG_POS      24

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int  u32;     /* int == long */
typedef unsigned long long u64;

#endif //SERVER_DISCOVER_DEF_H
