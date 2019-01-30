/* set tabstop=4 */
/********************************************************************************
 *                                                                              *
 * Copyright(C) 2004  Penta-Micro                                               *
 *                                                                              *
 * ALL RIGHT RESERVED                                                           *
 *                                                                              *
 * This software is the property of Penta-Micro and is furnished under          *
 * license by Penta-Micro. This software may be used only in accordance         *	
 * with the terms of said license. This copyright notice may not be             *
 * removed, modified or obliterated without the prior written permission        *
 * of Penta-Micro.                                                              *
 *                                                                              *
 * This software may not be copyed, transmitted, provided to or otherwise       *
 * made available to any other person, company, corporation	or other entity     *
 * except as specified in the terms of said license.                            *
 *                                                                              *
 * No right, title, ownership or other interest in the software is hereby       *
 * granted or transferred.                                                      *
 *                                                                              *
 * The information contained herein is subject to change without notice and     *
 * should not be construed as a commitment by Penta-Micro.                      *
 *                                                                              *
 ********************************************************************************
 
  MODULE NAME:  OSD_TODO.H
  
  REVISION HISTORY:
  
  Date       Ver Name                  Description
  ---------- --- --------------------- -----------------------------------------
 01/03/2008 0.1 CheulBeck(whitefe)       Created 
 ...............................................................................
 
  DESCRIPTION:
  
  This Module contains definition for osd function.
  
 ...............................................................................
*/    
 
#ifndef _OSD_TODO_H
#define _OSD_TODO_H


/** ************************************************************************* ** 
 ** includes
 ** ************************************************************************* **/
#include <stdint.h>

#include "dlist.h"


/** ************************************************************************* ** 
 ** defines
 ** ************************************************************************* **/
#define OSD_MODE        0
#define OSD_TRANS	  0x00000001
#define VIDEO_HSIZE   720
#define FMH_OSD         ((VIDEO_HSIZE+0x1F)>>5)
#define FMH_OSE         ((VIDEO_HSIZE+0xFF)>>8)

#define SUCCESS         0

#define FIELD_SIZE  1472*60     // 4byte

#define OSD_COLOR_DARKSLATEBLUE		0x00483d8b
#define OSD_COLOR_ROYALBLUE			0x004169e1
#define OSD_COLOR_SLATEBLUE			0x006a5acd
#define OSD_COLOR_DARKBLUE			0x0000008b
#define OSD_COLOR_SKYBLUE				0x0087ceeb
#define OSD_COLOR_BLUE					0x000000ff
#define OSD_COLOR_MEDIUMPURPLE		0x009370db
#define OSD_COLOR_INDIANRED			0x00cd5c5c
#define OSD_COLOR_WHITE				0x00ffffff
#define OSD_COLOR_BLACK				0x00000000

#define OSD_COLOR_POWDERBLUE			0x00b0e0e6
#define OSD_COLOR_PALETURQUOISE		0x00afeeee
#define OSD_COLOR_LIGHTBLUE			0x00add8e6
#define OSD_COLOR_DEEPSKYBLUE			0x0000bfff
#define OSD_COLOR_CORNFLOWERBLUE		0x006495ed
#define OSD_COLOR_DODGERBLUE			0x001e90ff
#define OSD_COLOR_STEELBLUE			0x004682b4
#define OSD_COLOR_WHEAT				0x00f5deb3
#define OSD_COLOR_YELLOW				0x00ffff00
#define OSD_COLOR_RED					0x00ff0000
#define OSD_COLOR_GREEN				0x0000ff00
#define OSD_COLOR_ORANGE				0x00ffa500
#define OSD_COLOR_TRANS				0x00000000




/** ************************************************************************* ** 
 ** typedefs
 ** ************************************************************************* **/
typedef struct _rect
{    
	uint32_t     x;    
	uint32_t     y;    
	uint32_t     w;    
	uint32_t     h;
} st_rect;

// bitmap font struct.
typedef struct _bmpfont_hdr_t
{
	unsigned long   fontsize;
	unsigned long   glyphnum;
	unsigned long   glyphsize;
} BMPFONT_HDR;

typedef struct _bmpmetrics_t
{
	long    width;
	long    height;
	long    horiBearingX;
	long    horiBearingY;
	long    horiAdvance;
	long    vertBearingX;
	long    vertBearingY;
	long    vertAdvance;
} BMPFONT_METRICS;

typedef struct _bmpfont_glyph_t
{
	unsigned long   ascii;
	unsigned long   size;
	BMPFONT_METRICS metrics;
//  unsigned char   data[1];
} BMPFONT_GLYPH;

//----------------------------
// font
typedef struct __bmpfont_t
{
	BMPFONT_GLYPH   glyph;
	unsigned char   *data;
} BMPFONT;

typedef struct __at206x_font_t
{
	unsigned long       glyphnum;
	unsigned long       fontsize;
	BMPFONT             *font;
} AT206X_FONT;

typedef enum __fontsize_t
{
	FONT_SMALL,
	FONT_NORMAL,
	FONT_LARGE
//	FONT_HANYU
} FONTSIZE;

typedef struct _osdfont_t
{

	long width;
	long height;
	long size;
	long horiAdvance;
	unsigned char *data;

} OSDFONT;

extern AT206X_FONT  gfont[3];   // 0: small, 1: normal, 2: large


//----------------------------
// image

typedef struct __image_hdr_t
{
	uint32_t    num;
} IMAGE_HDR;

typedef struct __image_info_t
{
	uint32_t    idx;
	uint32_t    width;
	uint32_t    height;
	uint32_t    size;
	char        name[64];
} IMAGE_INFO;

typedef struct __ycbcr_image_t
{
	struct dlist	next;
	uint32_t		idx;
	uint32_t    	width;
	uint32_t    	height;
	uint32_t    	size;
	char 			name[64];
	unsigned char	*data;
} YCBCRIMAGE;

typedef struct __at206x_image_t
{
	uint32_t		num;
	struct dlist	head;
} AT206X_IMAGE;
  
/** ************************************************************************* ** 
 ** function prototypes
 ** ************************************************************************* **/
void 
rgb2ycbcr(
	uint8_t r, 
	uint8_t g, 
	uint8_t b, 
	int *y, 
	int *cb, 
	int *cr
	);
int 
draw_framebuffer(
	int posx, 
	int posy, 
	int height, 
	int width
	);
void 
osd_put_pixel(
	uint32_t v, 
	uint32_t h, 
	uint8_t y, 
	uint8_t cb, 
	uint8_t cr, 
	uint8_t tp, 
	uint8_t blk
	);
uint32_t *
get_ycbcr_image(
	const char *filename, 
	int *width, 
	int *height
	);
void 
osd_display_image(
	const char *filename, 
	int posx, 
	int posy, 
	uint8_t tp,
	uint8_t blk
	);
void 
osd_display_string(
	const char *str, 
	int x, 
	int y, 
	int fontsize, 
	uint32_t color, 
	int tp, 
	int blk
	);
void 
osd_display_long_string(
	const uint32_t *str, 
	uint32_t count,
	int x, 
	int y, 
	int fontsize, 
	uint32_t color, 
	int tp, 
	int blk
	);
int 
load_bmpfont(
	const char *fontfile, 
	int fontsize
	);
int 
free_bmpfont(
	void
	);
int 
free_imagerc(
	void
	);
int 
load_imagerc(
	const char *imgpath
	);
void 
osd_display_rect(
	uint32_t color, 
	int posx, 
	int posy, 
	int width, 
	int height
	);
void 
osd_save_image(
	uint8_t *buf, 
	int x, 
	int y, 
	int width, 
	int height
	);
void 
osd_restore_image(
	uint8_t *buf, 
	int x, 
	int y, 
	int width, 
	int height
	);
int 
osd_initialize(
	void
	);
int 
osd_release(
	void
	);

void 
clear_osd(
	uint32_t mode
	);
	
void osd_clear_rect(int x, int y, int w, int h);
	
int 
draw_1ch_osd(
	uint8_t ch
	);
int 
draw_4ch_osd(
	uint8_t page
	);
void 
draw_1chrecmark(
	uint8_t ch
	);
void 
draw_4chrecmark(
	void
	);

#endif /* _OSD_TODO_H */
