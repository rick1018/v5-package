#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/fb.h>
#include <linux/videodev2.h>
#include <sys/time.h>
#include <ion_mem_alloc.h>
#include <videoOutPort.h>

#define __SUNXI_DISPLAY2__ 1

#if defined (__SUNXI_DISPLAY2__)
#include <arm_neon.h>
#endif

#if defined (__SUNXI_DISPLAY__)
#include "sunxi_display_v1.h"
#elif defined (__SUNXI_DISPLAY2__)
#include "sunxi_display_v2.h"
#include <ion_mem_alloc.h>
#else
#error "no display header file include"
#endif

#define RET_OK 0
#define RET_FAIL -1
#define SCREEN_0 0
#define SCREEN_1 1

#define CHN_NUM 3
#define LYL_NUM 4
#define HLAY(chn, lyl) (chn*4+lyl)
#define HD2CHN(hlay) (hlay/4)
#define HD2LYL(hlay) (hlay%4)

#define ZORDER_MAX 11
#define ZORDER_MID 5
#define ZORDER_MIN 0

#define DISP_DEV_NAME		("/dev/disp")

/*#define DISP_DEBUG*/
#define DISP_CLAR(x)	memset(&(x),  0, sizeof(x))
static unsigned char DISP_LOG = 0;
#define DISP_DBG_LOG	 if(DISP_LOG)  printf

unsigned int mLayerStatus[CHN_NUM][LYL_NUM];

static long long GetNowUs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000000ll + tv.tv_usec;
}

#if defined (__SUNXI_DISPLAY2__)
static void neon_nv21_rotage90(unsigned int width,
		unsigned int height, unsigned char* src_addr,
		unsigned char* dst_addr)
{
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int k = 0;

	unsigned int row_bytes = width;
	unsigned int col_bytes = height;
	unsigned int row_bytes_7 = width*7;
	unsigned int row_bytes_3 = width*3;

	unsigned char* y_src_line0 = src_addr;
	unsigned char* y_src_line1 = y_src_line0 + row_bytes;
	unsigned char* y_src_line2 = y_src_line1 + row_bytes;
	unsigned char* y_src_line3 = y_src_line2 + row_bytes;
	unsigned char* y_src_line4 = y_src_line3 + row_bytes;
	unsigned char* y_src_line5 = y_src_line4 + row_bytes;
	unsigned char* y_src_line6 = y_src_line5 + row_bytes;
	unsigned char* y_src_line7 = y_src_line6 + row_bytes;

	unsigned char* uv_src_line0 = src_addr + width*height;
	unsigned char* uv_src_line1 = uv_src_line0 + row_bytes;
	unsigned char* uv_src_line2 = uv_src_line1 + row_bytes;
	unsigned char* uv_src_line3 = uv_src_line2 + row_bytes;

	unsigned char* y_dst_line0 = dst_addr + col_bytes - 8;
	unsigned char* uv_dst_line0 = dst_addr + width*height + col_bytes - 8;

	unsigned char* y_dst_line0_static = y_dst_line0;
	unsigned char* uv_dst_line0_static = uv_dst_line0;

	for (i = 0; i < height; i += 8) {
		for (j = 0; j < width; j += 8) {
			asm volatile
			(
				"mov			 r9,    %9			\n\t"
				 "pld		 [%0,#64]               \n\t"
				 "pld		 [%1,#64]               \n\t"
				 "pld		 [%2,#64]               \n\t"
				 "pld		 [%3,#64]               \n\t"
				 "pld		 [%4,#64]               \n\t"
				 "pld		 [%5,#64]               \n\t"
				 "pld		 [%6,#64]               \n\t"
				 "pld		 [%7,#64]               \n\t"
				"vld1.u8		 {d0},  [%0]!			\n\t"
				"vld1.u8		 {d1},  [%1]!			\n\t"
				"vld1.u8		 {d2},  [%2]!			\n\t"
				"vld1.u8		 {d3},  [%3]!			\n\t"
				"vld1.u8		 {d4},  [%4]!			\n\t"
				"vld1.u8		 {d5},  [%5]!			\n\t"
				"vld1.u8		 {d6},  [%6]!			\n\t"
				"vld1.u8		 {d7},  [%7]!			\n\t"

				"vtrn.8			 d1,  d0				\n\t"
				"vtrn.8			 d3,  d2				\n\t"
				"vtrn.8			 d5,  d4				\n\t"
				"vtrn.8			 d7,  d6				\n\t"

				"vtrn.16		 d3,  d1				\n\t"
				"vtrn.16		 d2,  d0				\n\t"
				"vtrn.16		 d7,  d5				\n\t"
				"vtrn.16		 d6,  d4				\n\t"
				"vtrn.32		 d7,  d3				\n\t"
				"vtrn.32		 d5,  d1				\n\t"
				"vtrn.32		 d6,  d2				\n\t"
				"vtrn.32		 d4,  d0				\n\t"

			"vst1.u8		 {d7},  [%8], r9			\n\t"
			"vst1.u8		 {d6},  [%8], r9			\n\t"
			"vst1.u8		 {d5},  [%8], r9			\n\t"
			"vst1.u8		 {d4},  [%8], r9			\n\t"
			"vst1.u8		 {d3},  [%8], r9		\n\t"
			"vst1.u8		 {d2},  [%8], r9			\n\t"
			"vst1.u8		 {d1},  [%8], r9			\n\t"
			"vst1.u8		 {d0},  [%8], r9		\n\t"

			: "+r" (y_src_line0),  // %0
			  "+r" (y_src_line1),  // %1
			  "+r" (y_src_line2),  // %2
			  "+r" (y_src_line3),  // %3
			  "+r" (y_src_line4),  // %4
			  "+r" (y_src_line5),  // %5
			  "+r" (y_src_line6),  // %6
			  "+r" (y_src_line7),  // %7
			  "+r" (y_dst_line0),  // %8
			  "+r" (col_bytes)     // %9
			:
			: "cc", "memory", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "r9"
		    );
			asm volatile
			(
				"mov r8, %5			\n\t"

				"pld		 [%0,#64]				\n\t"
				"pld		 [%1,#64]				\n\t"
				"pld		 [%2,#64]				\n\t"
				"pld		 [%3,#64]				\n\t"
				"vld1.u8		 {d8}, [%0]!			\n\t"
				"vld1.u8		 {d9}, [%1]!			\n\t"
				"vld1.u8		 {d10}, [%2]!			\n\t"
				"vld1.u8		 {d11}, [%3]!			\n\t"

				"vtrn.16		 d9, d8				\n\t"
				"vtrn.16		 d11, d10			\n\t"
				"vtrn.32		 d11, d9			\n\t"
				"vtrn.32		 d10, d8			\n\t"

				"vst1.u8		 {d11}, [%4], r8			\n\t"
			"vst1.u8		 {d10}, [%4], r8			\n\t"
			"vst1.u8		 {d9}, [%4], r8			\n\t"
			"vst1.u8		 {d8}, [%4], r8			\n\t"

				: "+r" (uv_src_line0),  // %0
			  "+r" (uv_src_line1),  // %1
			  "+r" (uv_src_line2),  // %2
			  "+r" (uv_src_line3),  // %3
			  "+r" (uv_dst_line0),  // %4
			  "+r" (col_bytes)      // %5
			:
			: "cc", "memory", "d8", "d9", "d10", "d11", "r8"
		    );
		}

		y_src_line0 = y_src_line7;
		y_src_line1 += row_bytes_7;
		y_src_line2 += row_bytes_7;
		y_src_line3 += row_bytes_7;
		y_src_line4 += row_bytes_7;
		y_src_line5 += row_bytes_7;
		y_src_line6 += row_bytes_7;
		y_src_line7 += row_bytes_7;

		uv_src_line0 = uv_src_line3;
		uv_src_line1 += row_bytes_3;
		uv_src_line2 += row_bytes_3;
		uv_src_line3 += row_bytes_3;

		y_dst_line0 = y_dst_line0_static - i - 8;
		uv_dst_line0 = uv_dst_line0_static - i - 8;
	}
}

static void neon_nv21_rotage270(unsigned int width,
		unsigned int height, unsigned char* src_addr,
		unsigned char* dst_addr)
{
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int k = 0;

	unsigned int row_bytes = width;
	unsigned int col_bytes = height;

	unsigned int row_bytes_7 = width*7;
	unsigned int row_bytes_3 = width*3;

	unsigned char* y_src_line0 = src_addr;
	unsigned char* y_src_line1 = y_src_line0 + row_bytes;
	unsigned char* y_src_line2 = y_src_line1 + row_bytes;
	unsigned char* y_src_line3 = y_src_line2 + row_bytes;
	unsigned char* y_src_line4 = y_src_line3 + row_bytes;
	unsigned char* y_src_line5 = y_src_line4 + row_bytes;
	unsigned char* y_src_line6 = y_src_line5 + row_bytes;
	unsigned char* y_src_line7 = y_src_line6 + row_bytes;

	unsigned char* uv_src_line0 = src_addr + width*height;
	unsigned char* uv_src_line1 = uv_src_line0 + row_bytes;
	unsigned char* uv_src_line2 = uv_src_line1 + row_bytes;
	unsigned char* uv_src_line3 = uv_src_line2 + row_bytes;

	unsigned char* y_dst_line0 = dst_addr + col_bytes*(row_bytes - 8);
	unsigned char* uv_dst_line0 = dst_addr + width*height + col_bytes*(row_bytes - 4)/2;

	unsigned char* y_dst_line0_static = y_dst_line0;
	unsigned char* uv_dst_line0_static = uv_dst_line0;

	for (i = 0; i < height; i += 8) {
		for (j = 0; j < width; j += 8) {
			asm volatile
			(
				"mov			 r9,	%9			\n\t"
				 "pld		 [%0,#64]               \n\t"
				 "pld		 [%1,#64]               \n\t"
				 "pld		 [%2,#64]               \n\t"
				 "pld		 [%3,#64]               \n\t"
				 "pld		 [%4,#64]               \n\t"
				 "pld		 [%5,#64]               \n\t"
				 "pld		 [%6,#64]               \n\t"
				 "pld		 [%7,#64]               \n\t"

				"vld1.u8		 {d0},  [%0]!			\n\t"
				"vld1.u8		 {d1},  [%1]!			\n\t"
				"vld1.u8		 {d2},  [%2]!			\n\t"
				"vld1.u8		 {d3},  [%3]!			\n\t"
				"vld1.u8		 {d4},  [%4]!			\n\t"
				"vld1.u8		 {d5},  [%5]!			\n\t"
				"vld1.u8		 {d6},  [%6]!			\n\t"
				"vld1.u8		 {d7},  [%7]!			\n\t"

				"vtrn.8			 d0,  d1							\n\t"
				"vtrn.8			 d2,  d3							\n\t"
				"vtrn.8			 d4,  d5							\n\t"
				"vtrn.8			 d6,  d7							\n\t"

				"vtrn.16		 d1,  d3							\n\t"
				"vtrn.16		 d0,  d2							\n\t"
				"vtrn.16		 d5,  d7							\n\t"
				"vtrn.16		 d4,  d6							\n\t"

				"vtrn.32		 d3,  d7							\n\t"
				"vtrn.32		 d1,  d5							\n\t"
				"vtrn.32		 d2,  d6							\n\t"
				"vtrn.32		 d0,  d4							\n\t"

			"vst1.u8		 {d7},  [%8], r9			\n\t"
			"vst1.u8		 {d6},  [%8], r9			\n\t"
			"vst1.u8		 {d5},  [%8], r9			\n\t"
			"vst1.u8		 {d4},  [%8], r9			\n\t"
			"vst1.u8		 {d3},  [%8], r9		\n\t"
			"vst1.u8		 {d2},  [%8], r9			\n\t"
			"vst1.u8		 {d1},  [%8], r9			\n\t"
			"vst1.u8		 {d0},  [%8], r9		\n\t"

			: "+r" (y_src_line0),  // %0
			  "+r" (y_src_line1),  // %1
			  "+r" (y_src_line2),  // %2
			  "+r" (y_src_line3),  // %3
			  "+r" (y_src_line4),  // %4
			  "+r" (y_src_line5),  // %5
			  "+r" (y_src_line6),  // %6
			  "+r" (y_src_line7),  // %7
			  "+r" (y_dst_line0),  // %8
			  "+r" (col_bytes)     // %9
			:
			: "cc", "memory", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "r9"
		    );

			asm volatile
			(
				"mov			 r8,    %5			\n\t"

				"pld		 [%0,#64]               \n\t"
				"pld		 [%1,#64]               \n\t"
				"pld		 [%2,#64]               \n\t"
				"pld		 [%3,#64]               \n\t"

				"vld1.u8		 {d8},  [%0]!			\n\t"
				"vld1.u8		 {d9},  [%1]!			\n\t"
				"vld1.u8		 {d10}, [%2]!			\n\t"
				"vld1.u8		 {d11}, [%3]!			\n\t"

				"vtrn.16		 d8,  d9							\n\t"
				"vtrn.16		 d10,  d11							\n\t"

				"vtrn.32		 d9,  d11							\n\t"
				"vtrn.32		 d8,  d10							\n\t"

				"vst1.u8		 {d11}, [%4], r8			\n\t"
			"vst1.u8		 {d10}, [%4], r8			\n\t"
			"vst1.u8		 {d9},  [%4], r8			\n\t"
			"vst1.u8		 {d8},  [%4], r8			\n\t"

				: "+r" (uv_src_line0),  // %0
			  "+r" (uv_src_line1),  // %1
			  "+r" (uv_src_line2),  // %2
			  "+r" (uv_src_line3),  // %3
			  "+r" (uv_dst_line0),  // %4
			  "+r" (col_bytes)      // %5
			:
			: "cc", "memory", "d8", "d9", "d10", "d11", "r8"
		    );

			y_dst_line0 -= 16*col_bytes;
			uv_dst_line0 -= 8*col_bytes;
		}

		y_src_line0 = y_src_line7;
		y_src_line1 += row_bytes_7;
		y_src_line2 += row_bytes_7;
		y_src_line3 += row_bytes_7;
		y_src_line4 += row_bytes_7;
		y_src_line5 += row_bytes_7;
		y_src_line6 += row_bytes_7;
		y_src_line7 += row_bytes_7;

		uv_src_line0 = uv_src_line3;
		uv_src_line1 += row_bytes_3;
		uv_src_line2 += row_bytes_3;
		uv_src_line3 += row_bytes_3;

		y_dst_line0 = y_dst_line0_static + i + 8;

		uv_dst_line0 = uv_dst_line0_static + i + 8;
	}
}
#endif

static void c_nv21_rotage90(unsigned int width, unsigned int height,
unsigned char* src_addr, unsigned char* dst_addr)
{
#define NV21 0
#define YU12 1

	unsigned int i = 0;
	unsigned int j = 0;

	unsigned int k = 0;

	unsigned int format = NV21;
	unsigned int row_bytes = width;
	unsigned int col_bytes = height;
	unsigned char* y_src_line0 = src_addr;
	unsigned char* uv_src_line0 = src_addr + width*height;


	unsigned char* y_dst_line0 = dst_addr + col_bytes - 1;
	unsigned char* y_dst_line1 = y_dst_line0 + col_bytes;
	unsigned char* y_dst_line2 = y_dst_line1 + col_bytes;
	unsigned char* y_dst_line3 = y_dst_line2 + col_bytes;
	unsigned char* y_dst_line4 = y_dst_line3 + col_bytes;
	unsigned char* y_dst_line5 = y_dst_line4 + col_bytes;
	unsigned char* y_dst_line6 = y_dst_line5 + col_bytes;
	unsigned char* y_dst_line7 = y_dst_line6 + col_bytes;

	unsigned char* uv_dst_line0 = NULL;
	unsigned char* uv_dst_line0_v = NULL;
	unsigned char* uv_dst_line0_v_static = NULL;

	if (format == YU12) {
		uv_dst_line0 = dst_addr + width*height + col_bytes/2 - 1;
		uv_dst_line0_v = dst_addr + width*height*5/4 + col_bytes/2 - 1;

		uv_dst_line0_v_static = uv_dst_line0_v;
	} else if (format == NV21) {
		uv_dst_line0 = dst_addr + width*height + col_bytes - 2;
	}

	unsigned char* y_dst_line0_static = y_dst_line0;
	unsigned char* uv_dst_line0_static = uv_dst_line0;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			*(y_dst_line0) = *(y_src_line0++);
			y_dst_line0 += height;
		}
		y_dst_line0 = y_dst_line0_static - i - 1;
	}

	if (format == NV21) {
		for (i = 0; i < height/2; i++) {
			for (j = 0; j < width/2; j++) {
				*(uv_dst_line0) = *(uv_src_line0++);
				*(uv_dst_line0+1) = *(uv_src_line0++);
				uv_dst_line0 += height;
			}
			uv_dst_line0 = uv_dst_line0_static - 2*(i+1);
		}
	} else if (format == YU12) {
		for (i = 0; i < height/2; i++) {
			for (j = 0; j < width/2; j++) {
				*(uv_dst_line0) = *(uv_src_line0++);
				uv_dst_line0 += height/2;
			}
			uv_dst_line0 = uv_dst_line0_static - (i+1);
		}

		for (i = 0; i < height/2; i++) {
			for (j = 0; j < width/2; j++) {
				*(uv_dst_line0_v) = *(uv_src_line0++);
				uv_dst_line0_v += height/2;
			}
			uv_dst_line0_v = uv_dst_line0_v_static - (i+1);
		}
	}
}





static void c_nv21_rotage270(unsigned int width, unsigned int height,
	unsigned char* src_addr, unsigned char* dst_addr)
{
#define NV21 1
#define YU12 0

	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int k = 0;

	unsigned int format = NV21;
	//unsigned int format = YU12;

	unsigned int row_bytes = width;
	unsigned int col_bytes = height;

	unsigned char* y_src_line0 = src_addr;
	unsigned char* uv_src_line0 = src_addr + width*height;

	unsigned char* y_dst_line0 = dst_addr + col_bytes*(row_bytes - 1);
	unsigned char* y_dst_line1 = y_dst_line0 + col_bytes;
	unsigned char* y_dst_line2 = y_dst_line1 + col_bytes;
	unsigned char* y_dst_line3 = y_dst_line2 + col_bytes;
	unsigned char* y_dst_line4 = y_dst_line3 + col_bytes;
	unsigned char* y_dst_line5 = y_dst_line4 + col_bytes;
	unsigned char* y_dst_line6 = y_dst_line5 + col_bytes;
	unsigned char* y_dst_line7 = y_dst_line6 + col_bytes;

	unsigned char* uv_dst_line0 = NULL;
	unsigned char* uv_dst_line0_v = NULL;
	unsigned char* uv_dst_line0_v_static = NULL;

	if (format == YU12) {
		uv_dst_line0 = dst_addr + width*height;
		uv_dst_line0_v = dst_addr + width*height*5/4;

		uv_dst_line0_v_static = uv_dst_line0_v;
	} else if (format == NV21) {
		uv_dst_line0 = dst_addr + width*height + col_bytes*(row_bytes - 1)/2;
	}

	unsigned char* y_dst_line0_static = y_dst_line0;
	unsigned char* uv_dst_line0_static = uv_dst_line0;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			*(y_dst_line0) = *(y_src_line0++);
			y_dst_line0 -= height;
		}
		y_dst_line0 = y_dst_line0_static+ i + 1;
	}

	if (format == NV21) {
		for (i = 0; i < height/2; i++) {
			for (j = 0; j < width/2; j++) {
				*(uv_dst_line0) = *(uv_src_line0++);
				*(uv_dst_line0+1) = *(uv_src_line0++);
				uv_dst_line0 -= height;
			}
			uv_dst_line0 = uv_dst_line0_static + 2*(i+1);
		}
	} else if (format == YU12) {
		for (i = 0; i < height/2; i++) {
			for (j = 0; j < width/2; j++) {
				*(uv_dst_line0) = *(uv_src_line0++);
				uv_dst_line0 += height/2;
			}
			uv_dst_line0 = uv_dst_line0_static + (i+1);
		}

		for (i = 0; i < height/2; i++) {
			for (j = 0; j < width/2; j++) {
				*(uv_dst_line0_v) = *(uv_src_line0++);
				uv_dst_line0_v += height/2;
			}
			uv_dst_line0_v = uv_dst_line0_v_static + (i+1);
		}
	}
}

static int LayerConfig(int fd, __DISP_t cmd, disp_layer_config *pinfo)
{
	DISP_DBG_LOG("(%s %d)\n", __FUNCTION__, __LINE__);
	unsigned long args[4] = {0};
	unsigned int ret = RET_OK;

	args[0] = SCREEN_0;
    args[1] = (unsigned long)pinfo;
    args[2] = 1;
    ret = ioctl(fd, cmd, args);
	if(RET_OK != ret) {
		DISP_DBG_LOG("fail to get para\n");
		ret = RET_FAIL;
	}
	return ret;
}

static int LayerGetPara(int disp_fd, disp_layer_config *pinfo)
{
	return LayerConfig(disp_fd, DISP_LAYER_GET_CONFIG, pinfo);
}

static int LayerSetPara(int disp_fd, disp_layer_config *pinfo)
{
	return LayerConfig(disp_fd, DISP_LAYER_SET_CONFIG, pinfo);
}

static int LayerRender(int fd, unsigned int hlay, renderBuf *picture)
{
	int ret;
	disp_layer_config config;

	memset(&config, 0, sizeof(disp_layer_config));
	config.channel	= HD2CHN(hlay);
	config.layer_id = HD2LYL(hlay);
	LayerGetPara(fd, &config);
	config.info.fb.addr[0] = picture->y_phaddr;
	config.info.fb.addr[1] = picture->u_phaddr;
	config.info.fb.addr[2] = picture->v_phaddr;
	ret = LayerSetPara(fd, &config);

	return ret;
}

static int layer_set_src(int fd, unsigned int hlay, SrcInfo *src)
{
	unsigned long args[4] = {0};
	disp_layer_config config;

	DISP_DBG_LOG("(%s %d)\n", __FUNCTION__, __LINE__);

	memset(&config, 0, sizeof(disp_layer_config));
	config.channel  = HD2CHN(hlay);
	config.layer_id = HD2LYL(hlay);
	LayerGetPara(fd, &config);

	config.info.fb.crop.x = src->crop_x;
	config.info.fb.crop.y = src->crop_y;
	config.info.fb.crop.width  = (src->crop_w);
	config.info.fb.crop.height = (src->crop_h);

	DISP_DBG_LOG("width: 0x%llx, height: 0x%llx",
		config.info.fb.crop.width, config.info.fb.crop.height);

	config.info.fb.crop.x = config.info.fb.crop.x << 32;
	config.info.fb.crop.y = config.info.fb.crop.y << 32;
    config.info.fb.crop.width  = config.info.fb.crop.width << 32;
    config.info.fb.crop.height = config.info.fb.crop.height << 32;

	DISP_DBG_LOG("width: 0x%llx, height: 0x%llx\n",
		config.info.fb.crop.width, config.info.fb.crop.height);

	config.info.fb.size[0].width  = src->w;
	config.info.fb.size[0].height = src->h;
	config.info.fb.color_space = (disp_color_space)src->color_space;
	switch(src->format) {
		case VIDEO_PIXEL_FORMAT_YUV_PLANER_420:
		config.info.fb.format = DISP_FORMAT_YUV420_P;
		config.info.fb.size[1].width = src->w/2;
		config.info.fb.size[1].height = src->h/2;
		config.info.fb.size[2].width = src->w/2;
		config.info.fb.size[2].height = src->h/2;
		break;

        case VIDEO_PIXEL_FORMAT_YV12:
		config.info.fb.format = DISP_FORMAT_YUV420_P;
		config.info.fb.size[1].width = src->w/2;
		config.info.fb.size[1].height = src->h/2;
		config.info.fb.size[2].width = src->w/2;
		config.info.fb.size[2].height = src->h/2;
		break;

        case VIDEO_PIXEL_FORMAT_NV12:
		config.info.fb.format = DISP_FORMAT_YUV420_SP_UVUV;
		config.info.fb.size[1].width = src->w/2;
		config.info.fb.size[2].width = src->w/2;
		config.info.fb.size[1].height = src->h/2;
		config.info.fb.size[2].height = src->h/2;
		break;

        case VIDEO_PIXEL_FORMAT_NV21:
		config.info.fb.format = DISP_FORMAT_YUV420_SP_VUVU;
		config.info.fb.size[1].width = src->w/2;
		config.info.fb.size[2].width = src->w/2;
		config.info.fb.size[1].height = src->h/2;
		config.info.fb.size[2].height = src->h/2;
		break;

		default:
			config.info.fb.format = DISP_FORMAT_ARGB_8888;
			config.info.fb.size[1].width    = src->w;
			config.info.fb.size[1].height   = src->h;
			config.info.fb.size[2].width    = src->w;
			config.info.fb.size[2].height   = src->h;
			break;
	}
	DISP_DBG_LOG("set fb.format %d %d, color_space %d end\n",
		src->format, config.info.fb.format, config.info.fb.color_space);

	return LayerSetPara(fd, &config);
}

static int Yuv2YuvRotate(void *src, void *dest, SrcInfo *srcinfo, rotateDegree degree)
{
	unsigned int width, height;
	rotateDegree rot_angle;
	VideoPixelFormat fmt;

	if((!src) || (!dest)||(!srcinfo))
		return -1;

	rot_angle = degree;
	fmt = srcinfo->format;
	width = srcinfo->w;
	height = srcinfo->h;

	switch(rot_angle) {
		case ROTATION_ANGLE_90:
#if defined (__SUNXI_DISPLAY2__)
			neon_nv21_rotage90(width, height,
				(unsigned char*)src, (unsigned char*)dest);
#else

c			c_nv21_rotage90(width, height,
				(unsigned char*)src, (unsigned char*)dest);
#endif
			break;
		case ROTATION_ANGLE_270:
#if defined (__SUNXI_DISPLAY2__)
			neon_nv21_rotage270(width, height,
				(unsigned char*)src, (unsigned char*)dest);
#else

		c_nv21_rotage270(width, height,
				(unsigned char*)src, (unsigned char*)dest);
#endif
			break;
		case ROTATION_ANGLE_180:
		default:
			break;
	}
	return 0;
}

int DispAllocateVideoMem(void *hdl, videoParam *param)
{
    int pixelFormat;
    unsigned int nBufWidth;
    unsigned int nBufHeight;
    int index = 0;
    char* pVirBuf;
    char* pPhyBuf;
	int channel, layer_id;
	dispOutPort *vout_hd = (dispOutPort *)hdl;

	nBufWidth = param->srcInfo.w;
	nBufHeight = param->srcInfo.h;

		/* Native Window will malloc double buffer */
	pVirBuf = (char*)SunxiMemPalloc(vout_hd->pMemops,
				nBufWidth * nBufHeight * 3/2*2);
	pPhyBuf = (char*)SunxiMemGetPhysicAddressCpu(vout_hd->pMemops,
				pVirBuf);
		if ((NULL == pVirBuf) || (NULL == pPhyBuf)) {
			DISP_DBG_LOG("(%s %d) ION Malloc failed\n",
				__FUNCTION__, __LINE__);
			return RET_FAIL;
		}
	channel	= HD2CHN(vout_hd->hlayer);
	vout_hd->renderbuf[channel][0].phy_addr = (unsigned long)pPhyBuf;
	vout_hd->renderbuf[channel][0].vir_addr = (unsigned long)pVirBuf;
	vout_hd->renderbuf[channel][0].y_phaddr = (unsigned long)pPhyBuf;
	vout_hd->renderbuf[channel][0].u_phaddr = (unsigned long)(pPhyBuf + nBufWidth * nBufHeight);
	vout_hd->renderbuf[channel][0].v_phaddr = vout_hd->renderbuf[channel][0].u_phaddr
				+ (nBufWidth * nBufHeight)/4;

	vout_hd->renderbuf[channel][1].phy_addr = (unsigned long)(pPhyBuf + nBufWidth * nBufHeight * 3/2);
	vout_hd->renderbuf[channel][1].vir_addr = (unsigned long)(pVirBuf + nBufWidth * nBufHeight * 3/2);
	vout_hd->renderbuf[channel][1].y_phaddr = (unsigned long)(pPhyBuf + nBufWidth * nBufHeight * 3/2);
	vout_hd->renderbuf[channel][1].u_phaddr = vout_hd->renderbuf[channel][1].y_phaddr
				+ nBufWidth * nBufHeight;
	vout_hd->renderbuf[channel][1].v_phaddr = vout_hd->renderbuf[channel][1].u_phaddr
				+ (nBufWidth * nBufHeight)/4;
	vout_hd->interBufSet[channel] = 1;
	return RET_OK;
}

int DispFreeVideoMem(void *hdl)
{
	int channel;

	dispOutPort *vout_hd = (dispOutPort *)hdl;
	channel	= HD2CHN(vout_hd->hlayer);
    SunxiMemPfree(vout_hd->pMemops, (void *)vout_hd->renderbuf[channel][0].vir_addr);
	vout_hd->interBufSet[channel] = 0;

	return RET_OK;
}

int DispWriteData(void *hdl, void *data, int size, videoParam *param)
{
    unsigned int nBufWidth;
    unsigned int nBufHeight;
	int channel;
    int index = 0;
    char* pVirBuf;
    char* pPhyBuf;
	renderBuf renderbuf;
#ifdef DISP_DEBUG
	long long start_tm, end_tm;
#endif
	SrcInfo src_info;
	dispOutPort *vout_hd = (dispOutPort *)hdl;

	if (!data)
		return -1;

#ifdef DISP_DEBUG
	start_tm = GetNowUs();
#endif
	channel= HD2CHN(vout_hd->hlayer);
	nBufWidth = param->srcInfo.w;
	nBufHeight = param->srcInfo.h;
	if ((nBufWidth <= 0) || (nBufHeight <=0)
		||(param->srcInfo.crop_w <=0)
		||(param->srcInfo.crop_h <= 0)) {
		DISP_DBG_LOG("(%s %d) Bad parameter\n",
			__FUNCTION__, __LINE__);
		return RET_FAIL;
	}

	if (nBufWidth * nBufHeight * 3/2 > size)
		return -1;

	if (vout_hd->interBufSet[channel]) {
		index = (vout_hd->bufindex[channel] == 1) ? 1 : 0;
		pVirBuf = (char *)vout_hd->renderbuf[channel][index].vir_addr;
		pPhyBuf = (char *)vout_hd->renderbuf[channel][index].phy_addr;
		vout_hd->bufindex[channel] = (vout_hd->bufindex[channel] == 1) ? 0 : 1;
	} else {
		DISP_DBG_LOG("(%s %d) No Video memory\n",
			__FUNCTION__, __LINE__);

		return RET_FAIL;
	}
	memset(&src_info, 0, sizeof(SrcInfo));
	switch(vout_hd->route){
		case VIDEO_SRC_FROM_ISP:
		case VIDEO_SRC_FROM_CAM:
		case VIDEO_SRC_FROM_FILE:
			if ((vout_hd->rotate == ROTATION_ANGLE_90)
				||(vout_hd->rotate == ROTATION_ANGLE_270)) {
				Yuv2YuvRotate(data, pVirBuf,
					&param->srcInfo, vout_hd->rotate);
				src_info.w = param->srcInfo.h;
				src_info.h = param->srcInfo.w;
				src_info.crop_x = param->srcInfo.crop_x;
				src_info.crop_y= param->srcInfo.crop_y;
				src_info.crop_w = param->srcInfo.crop_h;
				src_info.crop_h = param->srcInfo.crop_w;
				src_info.format = param->srcInfo.format;
				src_info.color_space = param->srcInfo.color_space;
			} else {
				src_info.w = param->srcInfo.w;
				src_info.h = param->srcInfo.h;
				src_info.crop_x = param->srcInfo.crop_x;
				src_info.crop_y= param->srcInfo.crop_y;
				src_info.crop_w = param->srcInfo.crop_w;
				src_info.crop_h = param->srcInfo.crop_h;
				src_info.format = param->srcInfo.format;
				src_info.color_space = param->srcInfo.color_space;
				memcpy(pVirBuf, data, nBufWidth * nBufHeight * 3/2);
			}
			break;
		case VIDEO_SRC_FROM_VE:
			if ((vout_hd->rotate == ROTATION_ANGLE_90)
				||(vout_hd->rotate == ROTATION_ANGLE_270)) {
				Yuv2YuvRotate(data, pVirBuf,
					&param->srcInfo, vout_hd->rotate);
				src_info.w = param->srcInfo.h;
				src_info.h = param->srcInfo.w;
				src_info.crop_x = param->srcInfo.crop_x;
				src_info.crop_y= param->srcInfo.crop_y;
				src_info.crop_w = param->srcInfo.crop_h;
				src_info.crop_h = param->srcInfo.crop_w;
				src_info.format = param->srcInfo.format;
				src_info.color_space = param->srcInfo.color_space;
			} else {
				src_info.w = param->srcInfo.w;
				src_info.h = param->srcInfo.h;
				src_info.crop_x = param->srcInfo.crop_x;
				src_info.crop_y= param->srcInfo.crop_y;
				src_info.crop_w = param->srcInfo.crop_w;
				src_info.crop_h = param->srcInfo.crop_h;
				src_info.format = param->srcInfo.format;
				src_info.color_space = param->srcInfo.color_space;
				memcpy(pVirBuf, data, nBufWidth * nBufHeight * 3/2);
			}
			break;
		default:
			DISP_DBG_LOG("(%s %d) Not support disp src\n",
			__FUNCTION__, __LINE__);
			break;
	}
	SunxiMemFlushCache(vout_hd->pMemops, pVirBuf, nBufWidth * nBufHeight*3/2);
	renderbuf.y_phaddr = (unsigned long)pPhyBuf;
	if (src_info.format == VIDEO_PIXEL_FORMAT_YV12) {
		renderbuf.v_phaddr = (unsigned long)(pPhyBuf + nBufWidth * nBufHeight);
		renderbuf.u_phaddr = renderbuf.v_phaddr + (nBufWidth * nBufHeight)/4;
	} else {
		renderbuf.u_phaddr = (unsigned long)(pPhyBuf + nBufWidth * nBufHeight);
		renderbuf.v_phaddr = renderbuf.u_phaddr + (nBufWidth * nBufHeight)/4;
	}
	layer_set_src(vout_hd->disp_fd, vout_hd->hlayer, &src_info);
	LayerRender(vout_hd->disp_fd, vout_hd->hlayer, &renderbuf);
#ifdef DISP_DEBUG
	end_tm = GetNowUs();
	DISP_DBG_LOG("%s cost %lld us, pts:%lld\n",
			__FUNCTION__, end_tm - start_tm, start_tm);
#endif
	return RET_OK;
}

int DispDequeue(void *hdl, renderBuf *rBuf)
{
	int channel;
    int index = 0;
    char *pVirBuf;
    char *pPhyBuf;

	dispOutPort *vout_hd = (dispOutPort *)hdl;
	channel= HD2CHN(vout_hd->hlayer);

	if (vout_hd->interBufSet[channel]) {
		index = (vout_hd->bufindex[channel] == 1) ? 1 : 0;
		pVirBuf = (char *)vout_hd->renderbuf[channel][index].vir_addr;
		pPhyBuf = (char *)vout_hd->renderbuf[channel][index].phy_addr;
		vout_hd->bufindex[channel] = (vout_hd->bufindex[channel] == 1) ? 0 : 1;
	} else {
		DISP_DBG_LOG("(%s %d) No Video memory\n",
			__FUNCTION__, __LINE__);

		return RET_FAIL;
	}

	rBuf->isExtPhy = VIDEO_USE_INTERN_ALLOC_BUF;
	rBuf->phy_addr = (unsigned long)pPhyBuf;
	rBuf->vir_addr = (unsigned long)pVirBuf;

	return RET_OK;
}

int DispQueueToDisplay(void *hdl, int size, videoParam *param, renderBuf *rBuf)
{
	dispOutPort *vout_hd = (dispOutPort *)hdl;
	unsigned int nBufWidth;
    unsigned int nBufHeight;
	renderBuf renderbuf;
	SrcInfo src_info;

	nBufWidth = param->srcInfo.w;
	nBufHeight = param->srcInfo.h;

	if (nBufWidth * nBufHeight * 3/2 > size)
		return -1;

	memset(&src_info, 0, sizeof(SrcInfo));
	switch(vout_hd->route){
		case VIDEO_SRC_FROM_ISP:
		case VIDEO_SRC_FROM_CAM:
		case VIDEO_SRC_FROM_FILE:
		case VIDEO_SRC_FROM_VE:
		default:
				src_info.w = param->srcInfo.w;
				src_info.h = param->srcInfo.h;
				src_info.crop_x = param->srcInfo.crop_x;
				src_info.crop_y= param->srcInfo.crop_y;
				src_info.crop_w = param->srcInfo.crop_w;
				src_info.crop_h = param->srcInfo.crop_h;
				src_info.format = param->srcInfo.format;
				src_info.color_space = param->srcInfo.color_space;
			DISP_DBG_LOG("(%s %d)default src mode\n",
			__FUNCTION__, __LINE__);
			break;
	}

	if (rBuf->isExtPhy == VIDEO_USE_EXTERN_ION_BUF ){
		renderbuf.y_phaddr = (unsigned long)rBuf->y_phaddr;
		renderbuf.u_phaddr = (unsigned long)(rBuf->u_phaddr);
		renderbuf.v_phaddr = (unsigned long)(rBuf->v_phaddr);
	} else {
		SunxiMemFlushCache(vout_hd->pMemops, rBuf->vir_addr, nBufWidth * nBufHeight*3/2);
		renderbuf.y_phaddr = (unsigned long)rBuf->phy_addr;
		if (src_info.format == VIDEO_PIXEL_FORMAT_YV12) {
			renderbuf.v_phaddr = (unsigned long)(rBuf->phy_addr + nBufWidth * nBufHeight);
			renderbuf.u_phaddr = renderbuf.v_phaddr + (nBufWidth * nBufHeight)/4;
		} else {
			renderbuf.u_phaddr = (unsigned long)(rBuf->phy_addr + nBufWidth * nBufHeight);
			renderbuf.v_phaddr = renderbuf.u_phaddr + (nBufWidth * nBufHeight)/4;
		}
	}

	layer_set_src(vout_hd->disp_fd, vout_hd->hlayer, &src_info);
	LayerRender(vout_hd->disp_fd, vout_hd->hlayer, &renderbuf);
	return 0;
}

int DispSetEnable(void *hdl, int enable)
{
	disp_layer_config config;
	dispOutPort *vout_hd = (dispOutPort *)hdl;

	memset(&config, 0, sizeof(disp_layer_config));
	config.channel	= HD2CHN(vout_hd->hlayer);
	config.layer_id = HD2LYL(vout_hd->hlayer);
	LayerGetPara(vout_hd->disp_fd, &config);

	config.enable = enable;
	//vout_hd->enable = enable;
	return LayerSetPara(vout_hd->disp_fd, &config);
}

int DispSetSrcRect(void *hdl, VoutRect *src_rect)
{
	disp_layer_config config;
	dispOutPort *vout_hd = (dispOutPort *)hdl;

	memset(&config, 0, sizeof(disp_layer_config));
	config.channel	= HD2CHN(vout_hd->hlayer);
	config.layer_id = HD2LYL(vout_hd->hlayer);
	LayerGetPara(vout_hd->disp_fd, &config);

	config.info.fb.crop.x = src_rect->x;
	config.info.fb.crop.y = src_rect->y;
	config.info.fb.crop.width  = (src_rect->width);
	config.info.fb.crop.height = (src_rect->height);

	DISP_DBG_LOG("width: 0x%llx, height: 0x%llx",
		config.info.fb.crop.width, config.info.fb.crop.height);

	config.info.fb.crop.x = config.info.fb.crop.x << 32;
	config.info.fb.crop.y = config.info.fb.crop.y << 32;
    config.info.fb.crop.width  = config.info.fb.crop.width << 32;
    config.info.fb.crop.height = config.info.fb.crop.height << 32;

	return LayerSetPara(vout_hd->disp_fd, &config);
}

int DispSetRect(void *hdl, VoutRect *rect)
{
	disp_layer_config config;
	dispOutPort *vout_hd = (dispOutPort *)hdl;

	memset(&config, 0, sizeof(disp_layer_config));
	config.channel	= HD2CHN(vout_hd->hlayer);
	config.layer_id = HD2LYL(vout_hd->hlayer);
	LayerGetPara(vout_hd->disp_fd, &config);

	config.info.screen_win.x      = rect->x;
	config.info.screen_win.y      = rect->y;
	config.info.screen_win.width  = rect->width;
	config.info.screen_win.height = rect->height;

	return LayerSetPara(vout_hd->disp_fd, &config);
}

int DispSetRotateAngel(void *hdl, int degree)
{
	int ret = RET_OK;
	dispOutPort *vout_hd = (dispOutPort *)hdl;
	rotateDegree rot_deg = (rotateDegree)degree;

	/*unsigned long args[4]={0};
	args[0] = SCREEN_0;
	args[1] = degree;
	ret = ioctl(vout_hd->disp_fd, DISP_ROTATION_SW_SET_ROT, args);
    */
	vout_hd->rotate = rot_deg;

	return ret;
}

int DispSetRoute(void *hdl, int route)
{
	dispOutPort *vout_hd = (dispOutPort *)hdl;
	vout_hd->route = (videoSource)route;
	return RET_OK;
}
int DispSetZorder(void *hdl, int zorder)
{
	disp_layer_config config;
	dispOutPort *vout_hd = (dispOutPort *)hdl;
    videoZorder layer_zorder = (videoZorder)zorder;
	if ((layer_zorder < VIDEO_ZORDER_TOP)
		||(layer_zorder < VIDEO_ZORDER_TOP)) {
		DISP_DBG_LOG("(%s)invalid zorder\n", __FUNCTION__);
		return RET_FAIL;
	}
	memset(&config, 0, sizeof(disp_layer_config));
	config.channel = HD2CHN(vout_hd->hlayer);
	config.layer_id = HD2LYL(vout_hd->hlayer);
	LayerGetPara(vout_hd->disp_fd, &config);
	switch(layer_zorder) {
		case VIDEO_ZORDER_TOP:
			config.info.zorder = ZORDER_MAX;
			break;
		case VIDEO_ZORDER_MIDDLE:
			config.info.zorder = ZORDER_MID;
			break;
		case VIDEO_ZORDER_BOTTOM:
			config.info.zorder = ZORDER_MIN;
			break;
		default:
			break;
	}
	return LayerSetPara(vout_hd->disp_fd, &config);
}
int DispGetScreenWidth(void *hdl)
{
	dispOutPort *vout_hd = (dispOutPort *)hdl;
	unsigned int ioctlParam[4];
	unsigned int width;

	DISP_CLAR(ioctlParam);
	ioctlParam[0] = SCREEN_0;
	ioctlParam[1] = 0;
	width = ioctl(vout_hd->disp_fd, DISP_GET_SCN_WIDTH, ioctlParam);

	return width;
}

int DispGetScreenHeight(void *hdl)
{
	dispOutPort *vout_hd = (dispOutPort *)hdl;
	unsigned int ioctlParam[4];
	unsigned int height;

	DISP_CLAR(ioctlParam);
	ioctlParam[0] = SCREEN_0;
	ioctlParam[1] = 0;
	height = ioctl(vout_hd->disp_fd, DISP_GET_SCN_HEIGHT, ioctlParam);

	return height;
}

int DispSetIoctl(void *hdl, unsigned int cmd,  unsigned long para)
{
	int ret = 0;
	dispOutPort *vout_hd = (dispOutPort *)hdl;
	unsigned int ioctlParam[4];
	unsigned int disp_cmd;
	DISP_CLAR(ioctlParam);
#ifdef __SUNXI_DISPLAY2__
	switch(cmd) {
		case DISP_CMD_SET_BRIGHTNESS:
			disp_cmd = DISP_LCD_SET_BRIGHTNESS;
			break;
		case DISP_CMD_SET_CONTRAST:
		case DISP_CMD_SET_HUE:
		case DISP_CMD_SET_SATURATION:
			break;
		case DISP_CMD_SET_VEDIO_ENHANCE_DEFAULT:
			disp_cmd = DISP_ENHANCE_SET_MODE;
			break;
	}
#else
	switch(cmd) {
		case DISP_CMD_SET_BRIGHTNESS:
			disp_cmd = DISP_LCD_SET_BRIGHTNESS;
			break;
		case DISP_CMD_SET_CONTRAST:
			disp_cmd = DISP_CMD_LAYER_SET_CONTRAST;
			break;
		case DISP_CMD_SET_HUE:
			disp_cmd = DISP_CMD_LAYER_SET_HUE;
			break;
		case DISP_CMD_SET_SATURATION:
			disp_cmd = DISP_CMD_LAYER_SET_SATURATION;
			break;
		case DISP_CMD_SET_VEDIO_ENHANCE_DEFAULT:
			disp_cmd = DISP_CMD_SET_ENHANCE_MODE;
			break;
	}
#endif
	ioctlParam[0] = SCREEN_0;
	ioctlParam[1] = 0;
	ret = ioctl(vout_hd->disp_fd, cmd, (void*)ioctlParam);
	if(ret < 0){
		DISP_DBG_LOG("%s: fail to open backlight!\r\n", __func__);
		ret = -2;
		goto errHdl;
	}
errHdl:
	return ret;
}
int LayerRequest(int *pCh, int *pId)
{
	DISP_DBG_LOG("(%s %d)\n", __FUNCTION__, __LINE__);
	int ch;
	int id;
	{
		for (id = 0; id < LYL_NUM; id++) {
			for (ch = 0; ch < CHN_NUM; ch++) {
				if (!(mLayerStatus[ch][id] & LAYER_STATUS_REQUESTED)) {
					mLayerStatus[ch][id] |= LAYER_STATUS_REQUESTED;
					goto out;
				}
			}
		}
	}
out:
	if ((ch == CHN_NUM) && (id == LYL_NUM)) {
		DISP_DBG_LOG("all layer used.");
		return RET_FAIL;
	}
	*pCh = ch;
	*pId = id;
	DISP_DBG_LOG("requested: ch:%d, id:%d\n", ch, id);
	return HLAY(ch, id);
}

int LayerClose(int fd, unsigned int hlay)
{
	disp_layer_config config;
	memset(&config, 0, sizeof(disp_layer_config));
	config.channel	= HD2CHN(hlay);
	config.layer_id = HD2LYL(hlay);
	LayerGetPara(fd, &config);

	config.enable = 0;
	return LayerSetPara(fd, &config);
}

int LayerRelease(int fd, unsigned int hlay)
{
	int chn = HD2CHN(hlay);
	int lyl = HD2LYL(hlay);
	int ret = LayerClose(fd, hlay);
	if (RET_OK == ret) {
		if (chn >=0 && lyl >=0 && mLayerStatus[chn][lyl]) {
			mLayerStatus[chn][lyl] = 0;
		}
	}
	return ret;
}

int DispInit(void *hdl, int enable, int rotate, VoutRect *rect)
{
	int ret = 0;
	int hlay;
	int ch, id;
	disp_layer_config config;
	dispOutPort *vout_hd = (dispOutPort *)hdl;

	vout_hd->disp_fd = open(DISP_DEV_NAME, O_RDWR);
	if(vout_hd->disp_fd < 0){
		DISP_DBG_LOG("%s:open disp handle error!\r\n", __func__);
		goto errHdl;
	}

	memset(&config, 0, sizeof(disp_layer_config));
	hlay = LayerRequest(&ch, &id);
	config.channel = ch;
	config.layer_id = id;
	config.enable = enable;
	config.info.screen_win.x = rect->x;
	config.info.screen_win.y = rect->y;
	config.info.screen_win.width	= rect->width;
	config.info.screen_win.height	= rect->height;
	config.info.mode = LAYER_MODE_BUFFER;
	config.info.alpha_mode = 0;
	config.info.alpha_value = 0x80;
	config.info.fb.flags = DISP_BF_NORMAL;
	config.info.fb.scan = DISP_SCAN_PROGRESSIVE;
	config.info.fb.color_space = (rect->height < 720)?DISP_BT601:DISP_BT709;
	config.info.zorder = HLAY(ch, id);
	LayerSetPara(vout_hd->disp_fd, &config);

	DISP_DBG_LOG("hlay:%d, zorder=%d,", hlay, config.info.zorder);

	vout_hd->hlayer = hlay;
	vout_hd->enable = enable;
	vout_hd->rotate = (rotateDegree)rotate;
	vout_hd->rect.x = rect->x;
	vout_hd->rect.y = rect->y;
	vout_hd->rect.width = rect->width;
	vout_hd->rect.height = rect->height;
	vout_hd->pMemops = GetMemAdapterOpsS();
    SunxiMemOpen(vout_hd->pMemops);

errHdl:
	return ret;
}

int DispDeinit(void *hdl)
{
	int ret = 0;
	dispOutPort *vout_hd = (dispOutPort *)hdl;
	LayerRelease(vout_hd->disp_fd, vout_hd->hlayer);

	ret = close(vout_hd->disp_fd);
	if(ret != 0){
		DISP_DBG_LOG("%s:close disp handle failed!\r\n", __func__);
		goto errHdl;
	}
    SunxiMemClose(vout_hd->pMemops);
errHdl:
	return ret;
}
dispOutPort *CreateVideoOutport(int index)
{
	dispOutPort *voutport = (dispOutPort *)malloc(sizeof(dispOutPort));
	if(voutport == NULL)
		return NULL;
	voutport->init = DispInit;
	voutport->deinit = DispDeinit;
	voutport->writeData = DispWriteData;
	voutport->dequeue = DispDequeue;
	voutport->queueToDisplay = DispQueueToDisplay;
	voutport->setEnable = DispSetEnable;
	voutport->setRect = DispSetRect;
	voutport->setRotateAngel = DispSetRotateAngel;
	voutport->getScreenWidth = DispGetScreenWidth;
	voutport->getScreenHeight = DispGetScreenHeight;
	voutport->setRoute = DispSetRoute;
	voutport->SetZorder= DispSetZorder;
	voutport->allocateVideoMem = DispAllocateVideoMem;
	voutport->freeVideoMem = DispFreeVideoMem;
	voutport->setSrcRect = DispSetSrcRect;
	voutport->setIoctl = DispSetIoctl;
	return voutport;
}
int DestroyVideoOutport(dispOutPort *hdl)
{
	if(hdl != NULL)
		free(hdl);
}
