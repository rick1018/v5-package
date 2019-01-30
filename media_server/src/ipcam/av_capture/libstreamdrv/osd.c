#include <stdio.h>		// printf(), fread(), fwrite(), fseek()
#include <stdlib.h>		// exit(), EXIT_FAILURE
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <byteswap.h>
#include <assert.h>

#include "osd.h"

AT206X_FONT		gfont[3];

AT206X_IMAGE	imgrc;

/** ************************************************************************* ** 
 ** application functions
 ** ************************************************************************* **/
void clear_osd(uint32_t mode)
{

#if 0
	int i;
	int h, w;
	
	if (mode) {
		at206x_osd_write(0, 0, 0, 1, 1);			
		return;
	}		

	st_rect ch_loc[4] = {
		{40,  30, 18*3, 18}, {380,  30, 18*3, 18},
		{40, (gpSP->gp.video_form) ? 300:250, 18*3, 18}, {380, (gpSP->gp.video_form) ? 300:250, 18*3, 18}
	};
	
	st_rect rec_loc[4] = {
		{246,  30, 18, 18}, {586,  30, 18, 18},
		{246, (gpSP->gp.video_form) ? 300:250, 18, 18}, {586, (gpSP->gp.video_form) ? 300:250, 18, 18}
	};

	if (OSD_MODE == 0) {
		for (i = 0; i < 4; i++) {
			osd_display_rect(OSD_TRANS, ch_loc[i].x, ch_loc[i].y, ch_loc[i].w, ch_loc[i].h);
			osd_display_rect(OSD_TRANS, rec_loc[i].x, rec_loc[i].y, rec_loc[i].w, rec_loc[i].h);
		}
	} else {
		// osd memory clear	
		for (h = 0; h <480; h++) {
			for (w = 0; w < 720; w++) {
				osd_put_pixel(h, w, 0, 0, 0, 3, 0);
			}
		}
	}
#endif	

}


int free_imagerc(void)
{
	YCBCRIMAGE	*image = NULL;
	struct dlist *tmp;

	for(tmp = imgrc.head.next; tmp != &imgrc.head;) {
		image = dlist_entry(tmp, YCBCRIMAGE, next);
		tmp = tmp->next;
		dlist_del(&image->next);
		free(image);
	}

	return 0;
}

int osd_release(void)
{

	free_imagerc();

	return SUCCESS;
}

int load_imagerc(const char *imgpath)
{
	int fd, res, i;
	IMAGE_HDR	hdr;
	IMAGE_INFO	info;
	YCBCRIMAGE	*img;

	fd = open(imgpath, O_RDONLY);
	if(fd < 0) {
		perror("open()");
		return -1;
	}

	res = read(fd, &hdr, sizeof(IMAGE_HDR));
	if(res != sizeof(IMAGE_HDR)) {
		perror("read()");
		return -1;
	}

	imgrc.num = hdr.num;

	for(i=0; i<hdr.num; i++) {
		res = read(fd, &info, sizeof(IMAGE_INFO));

		if(res != sizeof(IMAGE_INFO)) {
			perror("read()");
			return -1;
		}

		img = (YCBCRIMAGE *)malloc(sizeof(YCBCRIMAGE));
		if(img == NULL) {
			perror("malloc()");
			return -1;
		}

		img->idx 	= i;
		img->width 	= info.width;
		img->height = info.height;
		img->size 	= (info.width * info.height * sizeof(uint32_t));
		sprintf(img->name,"%s", info.name);

		img->data = (unsigned char *)malloc(img->size);
		if(img->data == NULL) {
			perror("malloc()");
			return -1;
		}

		res = read(fd, img->data, img->size);
		if(res != info.size) {
			perror("read()");
			return -1;
		}

		dlist_addto_head(&imgrc.head, &img->next);
	}

	close(fd);

	return 0;
}

int draw_framebuffer(int posx, int posy, int height, int width)
{

#if 0
	long  maxx, maxy, minx, miny;
	uint32_t  h, w;
	uint32_t fid, pos;
	uint32_t pixel;
	uint8_t flag;

	minx = max(0, posx);
	miny = max(0, posy);
	maxx = min(720, posx + width);
	maxy = min(480, posy + height);

	flag = 0;
	pixel = 0;
	

	for(h = miny; h<maxy; h++) {
		fid = h&0x1;
		for(w=minx; w<maxx; w++) {
			pos = (h >> 3)*((FMH_OSD<<5)*4) + (w>>3)*32 + (((h>>1)&0x3)*8) + (w&0x6) + ((~w)&0x1);

			flag++;

			if(flag == 1) {
				pixel |= osd_buffer[fid][pos] << 16;
			}
			if(flag == 2) {
				pixel |= osd_buffer[fid][pos];
				if(fid) 
					at206x_osd_write(0, (gpST->state_osd.dec[0].bot_addr*2048)+((pos/2)*4), pixel, 0, 0);
				else
					at206x_osd_write(0, (gpST->state_osd.dec[0].top_addr*2048)+((pos/2)*4), pixel, 0, 0);
				
				flag = 0;
				pixel = 0;
			}
		}
	}

	at206x_osd_write(0, 0, 0, 0, 1);	
#endif

	return 0;

}

int osd_initialize(void)
{

//	int h, w;

	// font load
	load_bmpfont("font_small.bin", FONT_SMALL);
	load_bmpfont("font_normal.bin", FONT_NORMAL);
	load_bmpfont("font_large.bin", FONT_LARGE);
	//load_bmpfont("font_hanyu.bin", FONT_HANYU);

	dlist_init(&imgrc.head);
	load_imagerc("image.bin");	

	// clear osd memory
//	clear_osd(1);

//	draw_framebuffer(0, 0, 480, 720);

	return SUCCESS;

}

int load_bmpfont(const char *fontfile, int fontsize)
{

	int fontfd, res, i;
	AT206X_FONT *ft;

	fontfd = open(fontfile, O_RDONLY);

	if(fontfd < 0)
	{

//		perror("open()");
		fprintf(stderr,"[%s] %s\n",__func__, strerror(errno));
		return -1;

	}

	ft = &gfont[fontsize];

	BMPFONT_HDR     hdr;
	BMPFONT_GLYPH   glyph;

	res = read(fontfd, &hdr, sizeof(BMPFONT_HDR));

	if(res != sizeof(BMPFONT_HDR))
	{

		perror("read()");
		return -1;

	}

	ft->glyphnum = hdr.glyphnum;
	ft->fontsize = hdr.fontsize;
	ft->font = (BMPFONT *)malloc(sizeof(BMPFONT) * hdr.glyphnum);

	if(ft->font == NULL)
	{

		perror("malloc()");
		return -1;

	}

	for(i=0; i<hdr.glyphnum; i++)
	{

		int j = 0;

		res = read(fontfd, &glyph, sizeof(BMPFONT_GLYPH));

		if(res != sizeof(BMPFONT_GLYPH))
		{

			perror("read()");
			return -1;

		}

		ft->font[i].glyph.metrics.width = glyph.metrics.width;
		ft->font[i].glyph.metrics.height = glyph.metrics.height;
		ft->font[i].glyph.metrics.horiBearingX = glyph.metrics.horiBearingX;
		ft->font[i].glyph.metrics.horiBearingY = glyph.metrics.horiBearingY;
		ft->font[i].glyph.metrics.horiAdvance = glyph.metrics.horiAdvance;
		ft->font[i].glyph.metrics.vertBearingX = glyph.metrics.vertBearingX;
		ft->font[i].glyph.metrics.vertBearingY = glyph.metrics.vertBearingY;
		ft->font[i].glyph.metrics.vertAdvance = glyph.metrics.horiAdvance;
		ft->font[i].glyph.ascii = glyph.ascii;
		ft->font[i].glyph.size = glyph.size;

		ft->font[i].data = (unsigned char *)malloc(ft->font[i].glyph.size);

		if( 0 == ft->font[ i ].data )
		{

			perror( "0 == ft->font[ i ].data or 0 == ft->font[ i ].data_yellow.\n" );
			return -1;

		}

		read(fontfd, ft->font[i].data, ft->font[i].glyph.size);

	}

	close(fontfd);

	return 0;

}

int free_bmpfont(void)
{

	free(gfont[0].font);
	free(gfont[1].font);
	free(gfont[2].font);

	return 0;

}
