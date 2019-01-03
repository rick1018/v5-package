#ifndef __SMARTLINK_SOUNDWAVES_H
#define __SMARTLINK_SOUNDWAVES_H

#include <tina_log.h>

#define ADT_STR 129

#if __cplusplus
extern "C" {
#endif

/* direct log message to your system utils, default printf */
//extern int debug_enable;
//#define LOGE(fmt, arg...) do {if (1 | debug_enable) TLOGE(fmt, ##arg);} while (0);
//#define LOGD(fmt, arg...) do {if (debug_enable) TLOGD(fmt, ##arg);} while (0);
#define LOGE(fmt, arg...) do { printf(fmt"\n", ##arg);} while (0);
#define LOGD(fmt, arg...) do { printf(fmt"\n", ##arg);} while (0);


#if __cplusplus
};  // extern "C"
#endif

#endif
