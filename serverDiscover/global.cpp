#include "global.h"

Global::Global()
{

}

Global::~Global()
{

}

void Global::Init()
{
    m_sptrLog = std::make_shared<MainLog>();
    m_sptrLog->init();
}

Global* Global::GetInstance()
{
    static Global global;
    return &global;
}

void Global::AddNode(std::string strIp, NodeConnect* pNodeConn)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    if(pNodeConn == nullptr)
    {
        return;
    }

    m_mapNode[strIp] = pNodeConn;
}

void Global::DelNode(std::string strIp)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    if(m_mapNode.find(strIp) != m_mapNode.end())
    {
        m_mapNode[strIp]->SendReqCloseNode();
    }
}

void Global::GetAllNode(Json::Value &jsonData)
{
    jsonData["node"] = Json::nullValue;
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for (const auto kv : m_mapNode) {
        jsonData["node"][kv.first] = kv.second->GetNodeName();
    }
}