#ifndef __V5_DATA__
#define __V5_DATA__

struct V5_DATA{
    int type;
    unsigned char buff[4096*2160*3];
    unsigned int len;
    uint64_t pts;
};

#endif
