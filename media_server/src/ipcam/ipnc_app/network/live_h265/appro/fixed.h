
#include "interf_rom.h"

//define MRDTX_enable
//#define ARM_ASM
#define BIT_0      0
#define BIT_1      1

#define PRMNO_MR475 17
#define PRMNO_MR515 19
#define PRMNO_MR59  19
#define PRMNO_MR67  19
#define PRMNO_MR74  19
#define PRMNO_MR795 23
#define PRMNO_MR102 39
#define PRMNO_MR122 57
#define PRMNO_MRDTX 5

/* number of parameters per modes (values must be <= MAX_PRM_SIZE!) */
extern const Word16 prmno[N_MODES] ;


/* number of parameters to first subframe */
#define PRMNOFSF_MR475 7
#define PRMNOFSF_MR515 7
#define PRMNOFSF_MR59  7
#define PRMNOFSF_MR67  7
#define PRMNOFSF_MR74  7
#define PRMNOFSF_MR795 8
#define PRMNOFSF_MR102 12
#define PRMNOFSF_MR122 18

/* number of parameters to first subframe per modes */
extern const Word16 prmnofsf[N_MODES - 1] ;
/* table of contents byte for each mode index */
extern UWord8 toc_byte[16]  ;


#define M            10   
#define NB_QUA_PITCH 16
#define NB_QUA_CODE 32

extern const Word16 lsp_init_data[M] ;
extern const Word16 qua_gain_pitch[NB_QUA_PITCH] ;
extern const Word16 qua_gain_code[NB_QUA_CODE*3] ;

#define MAX_32 (Word32)0x7fffffffL
#define MIN_32 (Word32)0x80000000L

#define MAX_16 (Word16)0x7fff
#define MIN_16 (Word16)0x8000


Word16 norm_s (Word16 var1);                    
Word16 div_s (Word16 var1, Word16 var2);   
#ifndef ARM_ASM
Word16 norm_l (Word32 L_var1);    
#else
__inline Word16 norm_l (Word32 L_var1); 
#endif

 
//Word32 Inv_sqrt ( Word32 L_x    );
void Log2 (Word32 L_x, Word16 *exponent,  Word16 *fraction  );

Word32 Pow2 ( Word16 exponent,  Word16 fraction   );
Word32 Pow22 (  Word16 fraction   );
void Log2_norm ( Word32 L_x,   Word16 exp, Word16 *exponent, Word16 *fraction );



