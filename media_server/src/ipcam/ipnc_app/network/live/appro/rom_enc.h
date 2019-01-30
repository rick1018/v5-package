
#ifndef AMRCODE_PUBLIC_H
#define AMRCODE_PUBLIC_H

#include "typedef.h"


#define  MAX_BYTES  32

/* type defines */
typedef enum 
{          MR475 = 0,
            MR515,            
            MR59,
            MR67,
            MR74,
            MR795,
            MR102,
            MR122, 
	    MRDTX,
           N_MODES    
 }Mode ;






/*******************************************************************
                         ****             Interface fucntion declarations  ******* 

********************************************************************/

int Speech_Encode_Frame_init (void**amrcoder, Mode mode);
void Speech_Encode_Frame_exit (void **amrcoder);
int Speech_Encode_Frame (void *amrcoder,  Mode mode,  Word16 *new_speech,   unsigned char* bitstr);



#endif


