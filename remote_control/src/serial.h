#ifndef __SERIAL__
#define __SERIAL__
#include <termios.h>
#define BUF_LEN 1024
#define MAX_SESSION 4 
#define MAXNO(__A__, __B__) ((__A__ > __B__) ? __A__ : __B__)
#define SERIAL_TIMEOUT 30
#define SOCKTIMEOUT 2

extern int serialPort;

typedef enum {
    TCP_SERVER = 0,
    TCP_CLIENT,
    UDP_MODE
}MODE_TYPE;

typedef enum {
    LISTEN = 0,
    CLOSE,
    LOADING
}SESSION_STATUS;
#endif
