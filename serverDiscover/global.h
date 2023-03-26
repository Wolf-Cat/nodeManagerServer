#ifndef SERVER_DISCOVER_GLOBAL_H
#define SERVER_DISCOVER_GLOBAL_H

#include <iostream>
#include <map>
#include <mutex>
#include "nodeConnect.h"
#include "json.h"
#include "mainLog.h"

class Global {
public:
    static Global* GetInstance();
    ~Global();

    void Init();

    void AddNode(std::string strIp, NodeConnect* pNodeConn);
    void DelNode(std::string strIp);
    void GetAllNode(Json::Value &jsonData);

private:
    Global();

private:
    std::shared_ptr<MainLog> m_sptrLog;
    std::mutex m_mutex;
    std::map<std::string, NodeConnect*> m_mapNode;
};

#endif //SERVER_DISCOVER_GLOBADDL_H
