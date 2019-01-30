

/*******************************************************************************
*
*      File                  : AmrCode_comm.h
*      Purpose           : Declaration of public interface of AMR-NB SPEECH CODER
        Date                 : 2006 -01-18   create
*
*********************************************************************************/


#ifndef AMRENC_INTERFACE_H
#define AMRENC_INTERFACE_H

#include "typedef.h"
//#include "vad.h"
#include "rom_enc.h"



#ifdef MMS_IO
#define AMR_MAGIC_NUMBER "#!AMR\n"
#define MAX_PACKED_SIZE (MAX_SERIAL_SIZE /8 + 2)
#endif


/* frame size in serial bitstream file (frame type + serial stream + flags) */
#define SERIAL_FRAMESIZE (1+MAX_SERIAL_SIZE+5)


/****** const defines *****/

#define L_TOTAL      320                     /* Total size of speech buffer.             */
#define L_WINDOW     240                  /* Window size in LP analysis               */
#define L_FRAME      160                     /* Frame size                               */
#define L_FRAME_BY2  80                   /* Frame size divided by 2                  */
#define L_SUBFR      40                       /* Subframe size                            */
#define L_CODE       40                        /* codevector length                        */
#define NB_TRACK     5                       /* number of tracks                         */
#define STEP         5                            /* codebook step size                       */
#define NB_TRACK_MR102  4              /* number of tracks mode mr102              */
#define STEP_MR102      4                   /* codebook step size mode mr102            */
#define M            10                             /* Order of LP filter                       */
#define MP1          (M+1)                      /* Order of LP filter + 1                   */
#define LSF_GAP      205                      /* Minimum distance between LSF after quantization; 50 Hz = 205                    */
#define LSP_PRED_FAC_MR122 21299 /* MR122 LSP prediction factor (0.65 Q15) */
#define AZ_SIZE       (4*M+4)              /* Size of array of LP filters in 4 subfr.s */
#define PIT_MIN_MR122 18                 /* Minimum pitch lag (MR122 mode)           */
#define PIT_MIN       20                        /* Minimum pitch lag (all other modes)      */
#define PIT_MAX       143                      /* Maximum pitch lag                        */
#define L_INTERPOL    (10+1)               /* Length of filter for interpolation       */
#define L_INTER_SRCH  4                     /* Length of filter for CL LTP search  interpolation                            */
        
#define MU       26214                   /* Factor for tilt compensation filter 0.8  */
#define AGC_FAC  29491              /* Factor for automatic gain control 0.9    */
        
#define L_NEXT       40                /* Overhead in LP analysis                  */
#define SHARPMAX  13017           /* Maximum value of pitch sharpening        */
#define SHARPMIN  0                   /* Minimum value of pitch sharpening        */
                                                                          
                                                                          
#define MAX_PRM_SIZE    57        /* max. num. of params                      */
#define MAX_SERIAL_SIZE 244     /* max. num. of serial bits                 */
                                                                          
#define GP_CLIP   15565                /* Pitch gain clipping = 0.95               */
#define N_FRAME   7                      /* old pitch gains in average calculation   */

#define EHF_MASK 0x0008             /* encoder homing frame pattern             */


#define LTPG_MEM_SIZE 5            /* number of stored past LTP coding gains + 1 */

#define DTX_HIST_SIZE 8
#define DTX_ELAPSED_FRAMES_THRESH (24 + 7 -1)
#define DTX_HANG_CONST 7         /* yields eight frames of SP HANGOVER  */

#define VAD1                        /* VAD Algorithm Selection Compile Switch */


#define FRAME_LEN 160        /* Length (samples) of the input frame          */
#define COMPLEN 9               /* Number of sub-bands used by VAD              */
#define INV_COMPLEN 3641  /* 1.0/COMPLEN*2^15                             */
#define LOOKAHEAD 40         /* length of the lookahead used by speech coder */

#define UNITY 512                /* Scaling used with SNR calculation            */
#define UNIRSHFT 6              /* = log2(MAX_16/UNITY)                         */

#define TONE_THR (Word16)(0.65*MAX_16) /* Threshold for tone detection   */

/* Constants for background spectrum update */
#define ALPHA_UP1   (Word16)((1.0 - 0.95)*MAX_16)         /* Normal update, upwards:   */
#define ALPHA_DOWN1 (Word16)((1.0 - 0.936)*MAX_16)    /* Normal update, downwards  */
#define ALPHA_UP2   (Word16)((1.0 - 0.985)*MAX_16)        /* Forced update, upwards    */
#define ALPHA_DOWN2 (Word16)((1.0 - 0.943)*MAX_16)     /* Forced update, downwards  */
#define ALPHA3      (Word16)((1.0 - 0.95)*MAX_16)             /* Update downwards          */
#define ALPHA4      (Word16)((1.0 - 0.9)*MAX_16)               /* For stationary estimation */
#define ALPHA5      (Word16)((1.0 - 0.5)*MAX_16)               /* For stationary estimation */

/* Constants for VAD threshold */
#define VAD_THR_HIGH 1260   /* Highest threshold                 */
#define VAD_THR_LOW  720     /* Lowest threshold                  */
#define VAD_P1 0                    /* Noise level for highest threshold */
#define VAD_P2 6300               /* Noise level for lowest threshold  */
#define VAD_SLOPE (Word16)(MAX_16*(float)(VAD_THR_LOW-VAD_THR_HIGH)/(float)(VAD_P2-VAD_P1))

/* Parameters for background spectrum recovery function */
#define STAT_COUNT 20                   /* threshold of stationary detection counter         */
#define STAT_COUNT_BY_2 10         /* threshold of stationary detection counter         */
#define CAD_MIN_STAT_COUNT 5    /* threshold of stationary detection counter         */

#define STAT_THR_LEVEL 184    /* Threshold level for stationarity detection        */
#define STAT_THR 1000              /* Threshold for stationarity detection              */

/* Limits for background noise estimate */
#define NOISE_MIN 40              /* minimum */
#define NOISE_MAX 16000        /* maximum */
#define NOISE_INIT 150            /* initial */

/* Constants for VAD hangover addition */
#define HANG_NOISE_THR 100
#define BURST_LEN_HIGH_NOISE 4
#define HANG_LEN_HIGH_NOISE 7
#define BURST_LEN_LOW_NOISE 5
#define HANG_LEN_LOW_NOISE 4

/* Thresholds for signal power */
#define VAD_POW_LOW (Word32)15000         /* If input power is lower,              .   VAD is set to 0                         */
#define POW_PITCH_THR (Word32)343040     /* If input power is lower, pitch       .     detection is ignored                    */
#define POW_COMPLEX_THR (Word32)15000  /* If input power is lower, complex      . flags  value for previous frame  is un-set  */
 

/* Constants for the filter bank */
#define LEVEL_SHIFT 0       /* scaling                                  */
#define COEFF3   13363       /* coefficient for the 3rd order filter     */
#define COEFF5_1 21955     /* 1st coefficient the for 5th order filter */
#define COEFF5_2 6390       /* 2nd coefficient the for 5th order filter */

/* Constants for pitch detection */
#define LTHRESH 4
#define NTHRESH 4

/* Constants for complex signal VAD  */
#define CVAD_THRESH_ADAPT_HIGH  (Word16)(0.6 * MAX_16)  /* threshold for adapt stopping high    */
#define CVAD_THRESH_ADAPT_LOW  (Word16)(0.5 * MAX_16)   /* threshold for adapt stopping low     */
#define CVAD_THRESH_IN_NOISE  (Word16)(0.65 * MAX_16)     /* threshold going into speech on  a short term basis                   */

#define CVAD_THRESH_HANG  (Word16)(0.70 * MAX_16)      /* threshold                            */
#define CVAD_HANG_LIMIT  (Word16)(100)                          /* 2 second estimation time             */
#define CVAD_HANG_LENGTH  (Word16)(250)                       /* 5 second hangover                    */

#define CVAD_LOWPOW_RESET (Word16) (0.40 * MAX_16)     /* init in low power segment            */
#define CVAD_MIN_CORR (Word16) (0.40 * MAX_16)               /* lowest adaptation value              */

#define CVAD_BURST 20                                                                         /* speech burst length for speech reset */
#define CVAD_ADAPT_SLOW (Word16)(( 1.0 - 0.98) * MAX_16)              /* threshold for slow adaption */
#define CVAD_ADAPT_FAST (Word16)((1.0 - 0.92) * MAX_16)                /* threshold for fast adaption */
#define CVAD_ADAPT_REALLY_FAST (Word16)((1.0 - 0.80) * MAX_16)  /* threshold for really fastadaption                    */



/* state variable */
typedef struct {
   
   Word16 bckr_est[COMPLEN];     /* background noise estimate                */
   Word16 ave_level[COMPLEN];   /* averaged input components for stationary . estimation                            */
   Word16 old_level[COMPLEN];    /* input levels of the previous frame       */
   Word16 sub_level[COMPLEN];   /* input levels calculated at the end of   a frame (lookahead)                   */
   Word16 a_data5[3][2];             /* memory for the filter bank               */
   Word16 a_data3[5];                 /* memory for the filter bank               */

   Word16 burst_count;               /* counts length of a speech burst          */
   Word16 hang_count;               /* hangover counter                         */
   Word16 stat_count;                 /* stationary counter                       */

   /* Note that each of the following three variables (vadreg, pitch and tone)
      holds 15 flags. Each flag reserves 1 bit of the variable. The newest
      flag is in the bit 15 (assuming that LSB is bit 1 and MSB is bit 16). */
   Word16 vadreg;                    /* flags for intermediate VAD decisions     */
   Word16 pitch;                        /* flags for pitch detection                */
   Word16 tone;                         /* flags for tone detection                 */
   Word16 complex_high;         /* flags for complex detection              */
   Word16 complex_low;           /* flags for complex detection              */

   Word16 oldlag_count, oldlag;     /* variables for pitch detection            */
 
   Word16 complex_hang_count;   /* complex hangover counter, used by VAD    */
   Word16 complex_hang_timer;   /* hangover initiator, used by CAD          */
    
   Word16 best_corr_hp;                 /* FIP filtered value Q15                   */ 

   Word16 speech_vad_decision;   /* final decision                           */
   Word16 complex_warning;          /* complex background warning               */

   Word16 sp_burst_count;             /* counts length of a speech burst incl  HO addition                              */
   Word16 corr_hp_fast;                  /* filtered value                           */ 
} vadState1;


/***** Defines ****/

#define		YES		1
#define		NO		0
#define		ON		1
#define		OFF		0
#define		TRUE		1
#define		FALSE		0

#define         FRM_LEN                 80
#define         DELAY                   24
#define         FFT_LEN                 128

#define         NUM_CHAN                16
#define         LO_CHAN                 0
#define         HI_CHAN                 15

#define         UPDATE_THLD             35
#define         HYSTER_CNT_THLD         6
#define         UPDATE_CNT_THLD         50

#define		SHIFT_STATE_0		0		       /* channel energy scaled as 22,9 */
#define		SHIFT_STATE_1		1		       /* channel energy scaled as 27,4 */

#define		NOISE_FLOOR_CHAN_0	512		/* 1.0    scaled as 22,9 */
#define		MIN_CHAN_ENRG_0		32		/* 0.0625 scaled as 22,9 */
#define		MIN_NOISE_ENRG_0	32		       /* 0.0625 scaled as 22,9 */
#define		INE_NOISE_0		8192		/* 16.0   scaled as 22,9 */
#define		FRACTIONAL_BITS_0	9		       /* used as input to fn10Log10() */

#define		NOISE_FLOOR_CHAN_1	16		/* 1.0    scaled as 27,4 */
#define		MIN_CHAN_ENRG_1		1		/* 0.0625 scaled as 27,4 */
#define		MIN_NOISE_ENRG_1	1		       /* 0.0625 scaled as 27,4 */
#define		INE_NOISE_1		256		       /* 16.0   scaled as 27,4 */
#define		FRACTIONAL_BITS_1	4		       /* used as input to fn10Log10() */

#define		STATE_1_TO_0_SHIFT_R	(FRACTIONAL_BITS_1-FRACTIONAL_BITS_0)	/* state correction factor */
#define		STATE_0_TO_1_SHIFT_R	(FRACTIONAL_BITS_0-FRACTIONAL_BITS_1)	/* state correction factor */

#define         HIGH_ALPHA              29491		/* 0.9 scaled as 0,15 */
#define         LOW_ALPHA               22938		/* 0.7 scaled as 0,15 */
#define         ALPHA_RANGE             (HIGH_ALPHA - LOW_ALPHA)
#define         DEV_THLD                7168		       /* 28.0 scaled as 7,8 */

#define         PRE_EMP_FAC             (-26214)	       /* -0.8 scaled as 0,15 */

#define         CEE_SM_FAC              18022		/* 0.55 scaled as 0,15 */
#define         ONE_MINUS_CEE_SM_FAC    14746  /* 0.45 scaled as 0,15 */

#define         CNE_SM_FAC              3277		       /* 0.1 scaled as 0,15 */
#define         ONE_MINUS_CNE_SM_FAC    29491  /* 0.9 scaled as 0,15 */

#define         FFT_HEADROOM            2


/*********************************************************************************
*                         DECLARATION OF PROTOTYPES
*********************************************************************************/
int vad1_init (vadState1 **st); 
int vad1_reset (vadState1 *st);
void vad1_exit (vadState1 **st);
Word16 vad1 (vadState1 *st,   Word16 in_buf[]  );


void	r_fft (Word16 *farray_ptr);


#define vadState vadState1


typedef enum 
{ 
	RX_SPEECH_GOOD = 0,
	RX_SPEECH_DEGRADED,
	RX_ONSET,
	RX_SPEECH_BAD,
	RX_SID_FIRST,
	RX_SID_UPDATE,
	RX_SID_BAD,
	RX_NO_DATA,
	RX_N_FRAMETYPES  
}RXFrameType;


/* Channel decoded frame type */
typedef enum 
{ 
	CHDEC_SID_FIRST = 0,
	CHDEC_SID_FIRST_INCOMPLETE,
	CHDEC_SID_UPDATE_INCOMPLETE,
	CHDEC_SID_UPDATE,
	CHDEC_SPEECH,
	CHDEC_SPEECH_ONSET,
	CHDEC_ESCAPE_MARKER,
	CHDEC_ESCAPE_DATA,
	CHDEC_NO_DATA 
}CHDECFrameType ;

/* Channel decoded frame quality */
typedef enum
{ 
	CHDEC_GOOD = 0,
	CHDEC_PROBABLY_DEGRADED,
	CHDEC_PROBABLY_BAD,
	CHDEC_BAD
} CHDECFrameQuality ;


typedef struct 
{
    Word16 past_rq[M];                    /* Past quantized prediction error, Q15 */

} Q_plsfState;

typedef struct 
{
  Word16 old_A[M + 1];                   /* Last A(z) for case of unstable filter */
} LevinsonState;      


 typedef struct
 {
    Word16 onset;                                   /* onset state,                   Q0  */
    Word16 prev_alpha;                          /* previous adaptor output,       Q15 */
    Word16 prev_gc;                               /* previous code gain,            Q1  */

    Word16 ltpg_mem[LTPG_MEM_SIZE]; /* LTP coding gain history,       Q13 */
                                                             /* (ltpg_mem[0] not used for history) */
} GainAdaptState;
 

typedef struct
{
   LevinsonState *levinsonSt;
} lpcState;


typedef struct
{

   /* Past LSPs */
   Word16 lsp_old[M];
   Word16 lsp_old_q[M];

   /* Quantization state */
   Q_plsfState *qSt;

} lspState;


typedef struct
{
   Word16 past_qua_en[4];               /* normal MA predictor memory,         Q10 */
                                                         /* (contains 20*log10(qua_err))            */
   Word16 past_qua_en_MR122[4];   /* MA predictor memory for MR122 mode, Q10 */
                                                         /* (contains log2(qua_err))                */
} gc_predState;
 

 
typedef struct
{
    Word16 sf0_exp_gcode0;
    Word16 sf0_frac_gcode0;
    Word16 sf0_exp_target_en;
    Word16 sf0_frac_target_en;
    Word16 sf0_exp_coeff[5];
    Word16 sf0_frac_coeff[5];
    Word16 *gain_idx_ptr;
    
    gc_predState     *gc_predSt;
    gc_predState     *gc_predUnqSt;
    GainAdaptState   *adaptSt;
} gainQuantState;


 /* state variable */
typedef struct
{
   /* counters */
   Word16 count;
   
   /* gain history Q11 */
   Word16 gp[N_FRAME];
   
} tonStabState;


typedef struct 
{
   Word16 T0_prev_subframe;   /* integer pitch lag of previous sub-frame */
} Pitch_frState;

/* state variable */
typedef struct 
{
   Word16 old_T0_med;
   Word16 ada_w;
   Word16 wght_flg; 
} pitchOLWghtState;



/* state variable */
typedef struct 
{
    Pitch_frState *pitchSt;
} clLtpState;




typedef struct 
{
  Word16 y2_hi;
  Word16 y2_lo;
  Word16 y1_hi;
  Word16 y1_lo;
  Word16 x0;
  Word16 x1;
} Pre_ProcessState;
 

 

typedef struct 
{
   /* Speech vector */
   Word16 old_speech[L_TOTAL];
   Word16 *speech, *p_window, *p_window_12k2;
   Word16 *new_speech;             /* Global variable */
   
   /* Weight speech vector */
   Word16 old_wsp[L_FRAME + PIT_MAX];
   Word16 *wsp;

   /* OL LTP states */
   Word16 old_lags[5];
   Word16 ol_gain_flg[2];

   /* Excitation vector */
   Word16 old_exc[L_FRAME + PIT_MAX + L_INTERPOL];
   Word16 *exc;

   /* Zero vector */
   Word16 ai_zero[L_SUBFR + MP1];
   Word16 *zero;

   /* Impulse response vector */
   Word16 *h1;
   Word16 hvec[L_SUBFR * 2];

   /* Substates */
   lpcState   *lpcSt;
   lspState   *lspSt;
   clLtpState *clLtpSt;
   gainQuantState  *gainQuantSt;
   pitchOLWghtState *pitchOLWghtSt;
   tonStabState *tonStabSt;
 

   /* Filter's memory */
   Word16 mem_syn[M], mem_w0[M], mem_w[M];
   Word16 mem_err[M + L_SUBFR], *error;

   Word16 sharp;
} cod_amrState;

typedef struct
{
    Pre_ProcessState *pre_state;
    cod_amrState   *cod_amr_state;

} Speech_Encode_FrameState;

typedef struct
{
  Speech_Encode_FrameState* SpeechEncodeFrameState;
  
  
}AmrNBCoder;



#endif


