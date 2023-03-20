#ifndef SERVER_DISCOVER_HANDLEEVENT_H
#define SERVER_DISCOVER_HANDLEEVENT_H

void HandleAccept(int epollfd, int listenfd);
void HandleRead(int epollfd, int fd, char* buf);
void HandleWrite(int epollfd, int fd, char* buf);

void AddEvent(int epollfd, int fd, int state);
void DeleteEvent(int epollfd, int fd, int state);
void ModifyEvent(int epollfd, int fd, int state);

void SedFdNoBlock(int fd);
void SendLoginToSerd(int fd);
void RecvLoginResp(int fd, char *buf);

#endif //SERVER_DISCOVER_HANDLEEVENT_H
