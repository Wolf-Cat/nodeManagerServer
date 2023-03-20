#ifndef SERVER_DISCOVER_THREADPOOL_H
#define SERVER_DISCOVER_THREADPOOL_H

#include <vector>
#include <thread>
#include <mutex>
#include <memory>
#include <queue>
#include "nodeConnect.h"

class Xthread;

class XThreadPool {
public:
    XThreadPool();
    ~XThreadPool();

    static XThreadPool* GetInstance();

    void Init(int nThreadCount = 3);
    void UnInit();
    void DispatchThread(NodeConnect *pNode);   //将任务分发到线程去执行

private:
    int m_nThreadCount = 0;

    int m_indexLastUseThread = -1;

    std::vector<std::shared_ptr<Xthread>> m_vecThread;

    std::mutex m_mutex;
};


#endif //SERVER_DISCOVER_THREADPOOL_H
