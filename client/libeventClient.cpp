#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include "mainLog.h"
#include "ClientConnect.h"

using namespace std;

MainLog g_log;

//仅仅触发一下登录
void ReadTerminal(int fd, short event, void* arg)
{
    char buf[1024] = {0};
    read(fd, buf, sizeof(buf));

    ClientConnect *pConn = (ClientConnect *)arg;
    if(pConn == nullptr)
    {
        return;
    }

    pConn->ReqLogin();
}

int main()
{
    signal(SIGPIPE, SIG_IGN);
    g_log.init();


    struct event_base *base = event_base_new();
    if(base == NULL)
    {
        return -1;
    }

    ClientConnect clientSerd(base);
    clientSerd.ConnectToSerd();

    struct event* ev = event_new(base, STDIN_FILENO, EV_READ, ReadTerminal, &clientSerd);
    event_add(ev, NULL);
    event_base_dispatch(base);
    event_base_free(base);
}
