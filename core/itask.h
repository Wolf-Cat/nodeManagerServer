#ifndef SERVER_DISCOVER_Itask_H
#define SERVER_DISCOVER_Itask_H

#include <event2/event.h>

enum TaskLevel {        //任务的优先级
    LEVEL_0 = 0,
    LEVEL_1 = 1,
    LEVEL_2 = 2
};

class Itask {
public:
    Itask(evutil_socket_t sock) : m_sock(sock) {}
    virtual ~Itask() {}

    bool operator < (const Itask& t) const     //小顶堆
    {
        return m_tLevel > t.GetTaskLevel();
    }

    void SetEventBase(struct event_base *pBase)
    {
        m_pBase = pBase;
    }

    void SetTaskLevel(TaskLevel tLevel)
    {
        m_tLevel = tLevel;
    }

    TaskLevel GetTaskLevel() const
    {
        return m_tLevel;
    }

    virtual void HandlerTask() = 0;

protected:
    struct event_base *m_pBase = NULL;
    evutil_socket_t m_sock = 0;
    TaskLevel m_tLevel = LEVEL_2;
};

#endif //SERVER_DISCOVER_Itask_H
