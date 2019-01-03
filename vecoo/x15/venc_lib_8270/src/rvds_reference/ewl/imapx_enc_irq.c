/**








**/

#include <imapx_system.h>
#include <cmnlib.h>
#include <interrupt.h>
#include <rtl_system.h>
#include <rtl_trace.h>
#include <imapx_enc_irq.h>


volatile int gVENCIntr=0;
static rtl_trace_callback_t gRt={0};


#ifdef _IMAPX_ARM1136JFS_
__irq
#endif
void venc_irq(void )
{
	volatile unsigned int val;
	volatile unsigned int *pVal = (unsigned int *)IMAPX_VENC_ADDR;

	intr_mask(IMAPX_VENC_INTR);

	val = pVal[1];
	//our hw spec show us that there are may be 9 kinds of interupption from our hw 
	//but now we only care the bit 0 
	//log_printf("pVal[1]=0x%x",val);
	if(val&0x01){
		pVal[1] &= (~0x01);
		//pVal[1] = 0;
		//log_printf("pVal[1]=0x%x",pVal[1]);
	}
	gVENCIntr = 1;

	intr_clear(IMAPX_VENC_INTR);
	intr_unmask(IMAPX_VENC_INTR);

}

void venc_irq_wait(void)
{
	venc_rt_wait_hw();
	while(gVENCIntr == 0);
	gVENCIntr = 0;
}

void venc_irq_init(void)
{
	intr_register(IMAPX_VENC_INTR, (INTR_ISR)venc_irq, 0);
	intr_clear(IMAPX_VENC_INTR);
	intr_enable(IMAPX_VENC_INTR);
}

void venc_irq_deinit(void)
{
	intr_disable(IMAPX_VENC_INTR);

	gRt.read_reg = NULL;
	gRt.write_reg = NULL;
	gRt.enable_hw = NULL;
	gRt.wait_hw_ready = NULL;	
}

void venc_register_rt(void *rt)
{
	if(rt != NULL){
		gRt.read_reg = ((rtl_trace_callback_t *)rt)->read_reg;
		gRt.write_reg = ((rtl_trace_callback_t *)rt)->write_reg;
		gRt.enable_hw = ((rtl_trace_callback_t *)rt)->enable_hw;
		gRt.wait_hw_ready = ((rtl_trace_callback_t *)rt)->wait_hw_ready;
	}
}

void venc_rt_write_reg(unsigned int addr, unsigned value)
{
	if(gRt.write_reg != NULL){
		gRt.write_reg(addr, value);
	}
}

void venc_rt_read_reg(unsigned int addr, unsigned value)
{
	if(gRt.read_reg != NULL){
		gRt.read_reg(addr, value);
	}
}

void venc_rt_enable_hw()
{
	if(gRt.enable_hw != NULL){
		gRt.enable_hw();
	}
}

void venc_rt_disable_hw()
{
}

void venc_rt_wait_hw()
{
	if(gRt.wait_hw_ready != NULL){
		gRt.wait_hw_ready();
	}
}

