#ifndef __SERIAL_THREAD__
#define __SERIAL_THREAD__

struct threadInfo {
    int idleTime;
    int group;
    int port;
    int localPort;
    char IP[128];
};

void thread_init(void);
void tcpClientThread(int);
#endif
