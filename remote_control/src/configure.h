#ifndef __CONFIGURE__
#define __CONFIGURE__
struct conf {
    char name[128];
    char data[128];
    struct conf *next;
};

void _setSessionStatus(int group, char *status, int user);
char* get_conf(char *);
int get_conf_toInt(char *);
void configure_init(char *);
#endif
