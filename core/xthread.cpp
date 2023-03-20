#include <unistd.h>
#include "xthread.h"
#include "mainLog.h"
#include "itask.h"

static void NotifyCallBack(evutil_socket_t fd, short which, void *arg)
{
    Xthread *t = (Xthread *)arg;
    t->Notify(fd, which);
}

//通知有任务需要处理
void Xthread::Notify(evutil_socket_t fd, short which)
{
    //采用默认的水平触发，只要没有接收完全，会再次进入
    char buf[2] = {0};

    //通知采用的是读取到一个字符 'c'，读取到了就代表有任务已经放入队列
    int nReadSize = read(fd, buf, 1);
    if(nReadSize <= 0)
    {
        return;
    }

    log_debug("Notify work thread id=%d context = %s", m_nId, buf);

    NodeConnect *pConnect = nullptr;
    m_mutex.lock();
    if(m_listconnect.empty())
    {
        m_mutex.unlock();
        return;
    }
    pConnect = m_listconnect.front();
    m_listconnect.pop_front();
    m_mutex.unlock();

    pConnect->HandlerConnect();
}

Xthread::Xthread(int nThreadId) :
    m_nId(nThreadId)
{

}

Xthread::~Xthread()
{

}


void Xthread::Start()
{
    m_pthread = std::make_shared<std::thread>(&Xthread::Run, this);
}

void Xthread::Stop()
{
    if(m_pthread != nullptr)
    {
        m_pthread->join();
        std::cout << "stop thread id = " << m_nId << std::endl;
    }
}

void Xthread::Run()
{
    std::cout << "start thread id = " << m_nId << std::endl;

    SetUpEvent();
    event_base_dispatch(m_base);
    event_base_free(m_base);
}

bool Xthread::SetUpEvent() {

    //创建管道， fds[0] 为读端， fds[1] 为写端
    int fds[2];

    if(pipe(fds) != 0)
    {
        std::cout << "创建管道失败" << std::endl;
        return false;
    }

    //创建libevent上下文（配置为无锁）
    event_config *ev_conf = event_config_new();
    event_config_set_flag(ev_conf, EVENT_BASE_FLAG_NOLOCK);
    m_base = event_base_new_with_config(ev_conf);
    event_config_free(ev_conf);

    if(m_base == NULL)
    {
        log_error("Creat base config failed in thread SetUp");
        return false;
    }

    m_fdPipeWirte = fds[1];

    //添加管道监听事件，用于主线程激活子线程去执行任务
    // 2. 也有一个作用： 如果没有事件的话event_dispatch函数会退出
    struct event *ev = event_new(m_base, fds[0], EV_READ | EV_PERSIST, NotifyCallBack, this);
    event_add(ev, NULL);

    return true;
}

void Xthread::AddConnect(NodeConnect *pNode) {
    if(nullptr == pNode)
    {
        return;
    }

    pNode->SetBase(m_base);

    m_mutex.lock();
    m_listconnect.push_back(pNode);
    m_mutex.unlock();
}

void Xthread::Active() {
    int nWriteSize = write(m_fdPipeWirte, "c", 1);
    if(nWriteSize <= 0)
    {
        log_error("write to pipe failed, Active thread failed");
    }
}
