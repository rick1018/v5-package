/****************************************************************************************
Portions of this file are derived from the following 3GPP standard:

    3GPP TS 26.073
    ANSI-C code for the Adaptive Multi-Rate (AMR) speech codec
    Available from http://www.3gpp.org

(C) 2009, 3GPP ,RS
****************************************************************************************/
 
#ifndef SP_ENC_H
#define SP_ENC_H
 
#include "interf_rom.h"

int Speech_Encode_Frame_reset (Speech_Encode_FrameState *st);


typedef  void (*EncoderMMS_fun)( Word16 *param, UWord8 *stream );
typedef void (*cod_amr_func)( cod_amrState *st,    Word16 new_speech[], Word16 ana[],  Word16 synth[] );
#endif


