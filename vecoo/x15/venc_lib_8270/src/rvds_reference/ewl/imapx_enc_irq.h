/**








**/

#ifndef _IMAPX_ENC_IRQ_H_
#define _IMAPX_ENC_IRQ_H_

#include "imapx_system.h"

#define IMAPX_VENC_ADDR	0x25100000	//VENC_BASE_ADDR, 0x20d00000
#define IMAPX_VENC_INTR	129	//IMAPX200_ENC, 129


void venc_irq_init(void);
void venc_irq_deinit(void);
void venc_irq_wait(void);
#ifdef _IMAPX_ARM1136JFS_
__irq
#endif
void venc_irq(void );


void venc_register_rt(void *rt);
void venc_rt_write_reg(unsigned int addr, unsigned value);
void venc_rt_read_reg(unsigned int addr, unsigned value);
void venc_rt_enable_hw();
void venc_rt_disable_hw();
void venc_rt_wait_hw();


#endif

