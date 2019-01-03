#ifndef __SERIAL_UART__
#define __SERIAL_UART__

void con_close(int ,struct termios *);
void set_baudrate(struct termios *,char *);
int open_console (char *);
int con_setting(int, int, int, int, int);
void forwarding(int,int);
void reset_usb(int, char *);
void forwarding(int console, int client);
#endif
