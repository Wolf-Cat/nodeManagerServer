#include "threadPool.h"
#include "xthread.h"

XThreadPool::XThreadPool()
{

}

XThreadPool::~XThreadPool()
{

}

XThreadPool *XThreadPool::GetInstance() {
    static XThreadPool pool;
    return &pool;
}

void XThreadPool::Init(int nThreadCount)
{
    m_nThreadCount = nThreadCount;
    m_indexLastUseThread = -1;
    for (int i = 0; i < m_nThreadCount; ++i)
    {
        std::shared_ptr<Xthread> pXthread = std::make_shared<Xthread>(i + 1);
        m_vecThread.push_back(pXthread);
        pXthread->Start();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void XThreadPool::UnInit() {
    for (auto iter : m_vecThread)
    {
        iter->Stop();
    }
}

void XThreadPool::DispatchThread(NodeConnect *pNode)
{
    if(nullptr == pNode)
    {
        return;
    }

    m_indexLastUseThread++;
    int index = m_indexLastUseThread % m_nThreadCount;
    std::shared_ptr<Xthread> pthread = m_vecThread[index];

    pthread->AddConnect(pNode);
    pthread->Active();
}

