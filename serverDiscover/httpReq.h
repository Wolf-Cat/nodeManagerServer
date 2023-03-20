#ifndef SERVER_DISCOVER_HTTPREQ_H
#define SERVER_DISCOVER_HTTPREQ_H

#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/http_struct.h>
#include <event2/buffer_compat.h>

void HttpRequestCallBack(struct evhttp_request* pRequest, void *arg);
void HttpReqGetAllNodesCb(struct evhttp_request* pRequest, void *arg);
void HttpReqStopNodeCb(struct evhttp_request* pRequest, void *arg);

#endif //SERVER_DISCOVER_HTTPREQ_H
