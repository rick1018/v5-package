//----------------------------------------------//
//	H264 NALU c source code						//
//----------------------------------------------//
//#include <linux/videodev2.h>
#include "nalu.h"

#if 1
void hexdump(void *mem, unsigned int len)
{
	unsigned int i, j;
	unsigned int HEXDUMP_COLS = 16;

	for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
	{
		/* print offset */
		if(i % HEXDUMP_COLS == 0)
		{
			printf("0x%06x: ", i);
		}

		/* print hex data */
		if(i < len)
		{
			printf("%02x ", 0xFF & ((char*)mem)[i]);
		}
		else /* end of block, just aligning for ASCII dump */
		{
			printf("   ");
		}

		/* print ASCII dump */
		if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
		{
			for(j = i - (HEXDUMP_COLS - 1); j <= i; j++)
			{
				if(j >= len) /* end of block, not really printing */
				{
					putchar(' ');
				}
				else if(isprint(((char*)mem)[j])) /* printable char */
				{
					putchar(0xFF & ((char*)mem)[j]);        
				}
				else /* other char */
				{
					putchar('.');
				}
			}
			putchar('\n');
		}
	}
}
#endif

unsigned char* FindNextH264StartCode(unsigned char *pBuf, unsigned char *pBuf_end)
{
    unsigned char zero_cnt = 0;
//    zero_cnt = 0;
	printf("[%s]%s(%d)\n",__FILE__,__func__,__LINE__);
//	hexdump(pBuf,16);

    while(1)
    {
        if(pBuf == pBuf_end)
        {
            break;
        }
        if(*pBuf == 0)
        {
            zero_cnt++;
        }
        else if((zero_cnt >= 3) && (*pBuf == 1))
        {
            zero_cnt = 0;
            pBuf++;
            break;
        }
        else
        {
            zero_cnt = 0;
        }
        pBuf++;
    }
    return pBuf;
}

unsigned int Ue(unsigned char *pBuff, unsigned int nLen, unsigned int *nStartBit)
{
	unsigned int i=0;
    unsigned int nZeroNum = 0;
	unsigned long dwRet = 0;
	
    while((*nStartBit) < (nLen * 8))
    {
        if(pBuff[(*nStartBit) / 8] & (0x80 >> ((*nStartBit) % 8)))
            break;

        nZeroNum++;
        (*nStartBit)++;
    }
    (*nStartBit) ++;

    for(i=0; i<nZeroNum; i++)
    {
        dwRet <<= 1;
        if((pBuff[(*nStartBit) / 8]) & (0x80 >> ((*nStartBit) % 8)))
        {
            dwRet += 1;
        }
        (*nStartBit)++;
    }
    return (1 << nZeroNum) - 1 + dwRet;
}

int Se(unsigned char *pBuff, unsigned int nLen, unsigned int *nStartBit)
{
#if 0
    int UeVal=Ue(pBuff,nLen,*nStartBit);
	double k=UeVal;
	int nValue=ceil(k/2);
    
    if(UeVal % 2==0)
        nValue=-nValue;

    return nValue;
#else
    int UeVal=Ue(pBuff,nLen,nStartBit);
	int nValue = UeVal / 2;
	if( nValue > 0) nValue += UeVal & 0x01;
	if(UeVal % 2==0)
		nValue=-nValue;

	return nValue;
#endif
}

unsigned long u(unsigned int BitCount,unsigned char * buf,unsigned int *nStartBit)
{
    unsigned long dwRet = 0;
	unsigned int i = 0;
	
	for(i=0; i<BitCount; i++)
    {
        dwRet <<= 1;
        if(buf[(*nStartBit) / 8] & (0x80 >> ((*nStartBit) % 8)))
        {
            dwRet += 1;
        }
        (*nStartBit)++;
    }

    return dwRet;
}

bool h264_decode_Seq_Parameter_Set(unsigned char *buffer, unsigned int nLen, int *Width, int *Height)
{
    unsigned int StartBit=0;
    int forbidden_zero_bit, nal_ref_idc, nal_unit_type;
	unsigned char *buf = FindNextH264StartCode(buffer,buffer+300);
	
	forbidden_zero_bit=u(1,buf,&StartBit);
    nal_ref_idc=u(2,buf,&StartBit);
    nal_unit_type=u(5,buf,&StartBit);
	
    if(nal_unit_type==7)
    {
		int profile_idc, constraint_set0_flag, constraint_set1_flag, constraint_set2_flag, constraint_set3_flag, reserved_zero_4bits, level_idc;
		int seq_parameter_set_id, log2_max_frame_num_minus4, pic_order_cnt_type;
		int num_ref_frames, gaps_in_frame_num_value_allowed_flag, pic_width_in_mbs_minus1, pic_height_in_map_units_minus1;
		
		profile_idc=u(8,buf,&StartBit);
        constraint_set0_flag=u(1,buf,&StartBit);//(buf[1] & 0x80)>>7;
        constraint_set1_flag=u(1,buf,&StartBit);//(buf[1] & 0x40)>>6;
        constraint_set2_flag=u(1,buf,&StartBit);//(buf[1] & 0x20)>>5;
        constraint_set3_flag=u(1,buf,&StartBit);//(buf[1] & 0x10)>>4;
        reserved_zero_4bits=u(4,buf,&StartBit);
        level_idc=u(8,buf,&StartBit);
		
        seq_parameter_set_id=Ue(buf,nLen,&StartBit);

        if((profile_idc == 100) || (profile_idc == 110) || (profile_idc == 122) || (profile_idc == 144))
        {
			int chroma_format_idc, residual_colour_transform_flag, bit_depth_luma_minus8, bit_depth_chroma_minus8, qpprime_y_zero_transform_bypass_flag;
			int seq_scaling_matrix_present_flag;
            
			chroma_format_idc=Ue(buf,nLen,&StartBit);
			residual_colour_transform_flag = 0;
			
            if(chroma_format_idc == 3)
                residual_colour_transform_flag=u(1,buf,&StartBit);

            bit_depth_luma_minus8=Ue(buf,nLen,&StartBit);
            bit_depth_chroma_minus8=Ue(buf,nLen,&StartBit);
            qpprime_y_zero_transform_bypass_flag=u(1,buf,&StartBit);
            seq_scaling_matrix_present_flag=u(1,buf,&StartBit);

            if(seq_scaling_matrix_present_flag)
            {
				int seq_scaling_list_present_flag[8];
				int i=0;
                for( i = 0; i < 8; i++ )
                {
                    seq_scaling_list_present_flag[i]=u(1,buf,&StartBit);
                }
            }
        }
        
		log2_max_frame_num_minus4=Ue(buf,nLen,&StartBit);
        pic_order_cnt_type=Ue(buf,nLen,&StartBit);
		
        if(pic_order_cnt_type == 0)
        {
            int log2_max_pic_order_cnt_lsb_minus4=Ue(buf,nLen,&StartBit);
        }
        else if(pic_order_cnt_type == 1)
        {
            int delta_pic_order_always_zero_flag=u(1,buf,&StartBit);
            int offset_for_non_ref_pic=Se(buf,nLen,&StartBit);
            int offset_for_top_to_bottom_field=Se(buf,nLen,&StartBit);
            int num_ref_frames_in_pic_order_cnt_cycle=Ue(buf,nLen,&StartBit);
#if 0
            int *offset_for_ref_frame=new int[num_ref_frames_in_pic_order_cnt_cycle];
            for(int i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++)
                offset_for_ref_frame[i]=Se(buf,nLen,&StartBit);

            delete [] offset_for_ref_frame;
#else
			int offset_for_ref_frame = 0;
			int i = 0;
            for(i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++)
                offset_for_ref_frame =Se(buf,nLen,&StartBit);
#endif

        }

        num_ref_frames=Ue(buf,nLen,&StartBit);
        gaps_in_frame_num_value_allowed_flag=u(1,buf,&StartBit);
        pic_width_in_mbs_minus1=Ue(buf,nLen,&StartBit);
        pic_height_in_map_units_minus1=Ue(buf,nLen,&StartBit);

        *Width=(pic_width_in_mbs_minus1+1)*16;
        *Height=(pic_height_in_map_units_minus1+1)*16;
//		char szmsg[100];
//		sprintf(szmsg, "nalu:%d, %d\n", Width, Height);
//		printf(szmsg);
		
        return true;
    }
    else
    {
		printf("nal_unit_type=%d\n",nal_unit_type);
        return false;
    }
}

/*
int next_nal_type(bit_context_t *context_p, int *type_ptr, int nBeAdded_H264StartCode)
{
	int				state;

	if(context_p->bit_idx != 7) {
		context_p->bit_idx = 7;
		++context_p->byte_idx;
	}

	if(context_p->byte_idx == context_p->byte_count) {
		return 0;
	}

	if	( nBeAdded_H264StartCode == 0 )
	{
		if(context_p->byte_idx == 0) {
			if(context_p->byte_count > 0) {
				context_p->byte_idx	= 1;
				*type_ptr	= (context_p->byte_ptr[0] & 0x1f);
				return 1;
			}
		}
	}

	state = 0;

	while(context_p->byte_idx < context_p->byte_count) {
		switch(state) {
		case	0:
			state	= (context_p->byte_ptr[context_p->byte_idx] == 0)		? 1
					: 0;
		break;

		case	1:
			state	= (context_p->byte_ptr[context_p->byte_idx] == 0)		? 2
					: 0;
		break;

		case	2:
			state	= (context_p->byte_ptr[context_p->byte_idx] == 0)		? 2
					: (context_p->byte_ptr[context_p->byte_idx] == 1)		? 3
					: 0;
		break;
		}

		++context_p->byte_idx;
		if(state == 3) {
			if(context_p->byte_idx == context_p->byte_count)
				return 0;
			*type_ptr	= context_p->byte_ptr[context_p->byte_idx] & 0x1f;
			++context_p->byte_idx;
			break;
		}
	}

	return (state == 3) ? 1:0;
}

int chk_frame_type( const int &codec_type, const unsigned char *buf, const long &len )
{
 
    if( H264 == codec_type )
    {
 
        unsigned char nal_type = buf[ 4 ] & 0xf;
 
        if( 7 == nal_type || 8 == nal_type || 5 == nal_type )//SPS, PPS and I-frame.
            return 1;
        else
            return 0;
 
    }
    else if( MPEG4 == codec_type )
    {
 
        unsigned char f_type = ( buf[ 4 ] & 0xc0 ) >> 6;
 
        if( 0 == f_type )//I-Frame
            return 1;
        else if( 1 == f_type )//P-frame
            return 0;
        else if( 2 == f_type )//B-frame
            return 0;
        else if( 3 == f_type )//S-frame
            return 0;
        else
            return -1;
 
    }
 
    return 0;
 
}
*/