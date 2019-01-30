/*
*****************************************************************************
*
*      GSM AMR-NB speech codec      2008/07/12,rslin
*                                               
*
*****************************************************************************
*
*      File             : amr.h
*      Purpose          : Scales the postfilter output on a subframe basis
*                       : by automatic control of the subframe gain.
*
*****************************************************************************
*/
#ifndef amr_h
#define amr_h "$Id $"
#include "rom_enc.h"
#define AMR_MAGIC_NUMBER "#!AMR\n"
typedef struct AMR_state_s
{
  void *speech_encoder_state;
 void * amrcoder ;
 Mode mode;


}AMR_state;

extern void AUDIO_AMR_Init(AMR_state *amr_ptr, int AMR_mode);
extern int AUDIO_AMR_Encode(short *rtsp_buf, short *src, short bufsize,AMR_state *amr_ptr);
extern void AUDIO_AMR_Close(AMR_state *amr_ptr);
#endif
