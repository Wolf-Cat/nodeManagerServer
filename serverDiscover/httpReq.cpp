#include "httpReq.h"
#include "json.h"
#include <string>
#include "global.h"

void HttpRequestCallBack(struct evhttp_request* pRequest, void *arg)
{
    const struct evhttp_uri* evhttpUri = evhttp_request_get_evhttp_uri(pRequest);
    char szUrl[8192] = {0};
    evhttp_uri_join(const_cast<struct evhttp_uri*>(evhttpUri), szUrl, sizeof(szUrl));

    struct evbuffer* evbuf = evbuffer_new();

    evbuffer_add_printf(evbuf, "Server response, is url = %s", szUrl);
    evhttp_send_reply(pRequest, HTTP_OK, "OK", evbuf);
    evbuffer_free(evbuf);
}

void HttpReqGetAllNodesCb(struct evhttp_request* pRequest, void *arg)
{
    Json::Value jsonData = Json::nullValue;
    Json::FastWriter jsonWriter;
    jsonData["res"] = Json::nullValue;
    jsonData["res"]["err_code"] = 0;
    jsonData["res"]["err_msg"] = "success";

    Global::GetInstance()->GetAllNode(jsonData);

    std::string strJson = jsonWriter.write(jsonData);

    struct evbuffer* evbuf = evbuffer_new();
    evbuffer_add_printf(evbuf, strJson.c_str());
    evhttp_send_reply(pRequest, HTTP_OK, "OK", evbuf);
    evbuffer_free(evbuf);
}

void HttpReqStopNodeCb(struct evhttp_request* pRequest, void *arg)
{
    Json::Value jsonData = Json::nullValue;
    Json::FastWriter jsonWriter;
    jsonData["res"] = Json::nullValue;
    jsonData["res"]["err_code"] = 0;
    jsonData["res"]["err_msg"] = "success";
}

