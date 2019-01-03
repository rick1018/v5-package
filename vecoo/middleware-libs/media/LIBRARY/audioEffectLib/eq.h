#ifndef _EQ_H_
#define _EQ_H_
#include <stdlib.h>
#ifdef FIXED_POINT

#else

#include <math.h>


#endif
/* filter type definition */
typedef enum
{
	/* low pass shelving filter */
	LOWPASS_SHELVING,
	/* band pass peak filter */
	BANDPASS_PEAK,
	/* high pass shelving filter */
	HIHPASS_SHELVING
}eq_ftype_t;

/* eq prms to create equalizer handle */
typedef struct
{
	/* boost/cut gain in dB */
	int G;
	/* cutoff/center frequency in Hz */
	int fc;
	/* quality factor */
	int	Q;
	/* filter type */
	eq_ftype_t type;
	/* sampling rate */
	int fs;
}eq_prms_t;	



/*
	function eq_create
description:
	use this function to create the equalizer object
prms:
	eq_prms_t: [in], desired frequency response array, should contain n items
	n:[in], items(biquad)num
returns:
	the equalizer handle
*/
void* eq_create(eq_prms_t* prms, int n);
/*
	function eq_process
description:
	equalizer processing function
prms:
	handle:[in,out] equalzier handle
	x:[in,out],	input signal
	len:[in], input length(in samples)
returns:
	none
*/
void eq_process(void* handle, short* x, int len);
/*
	function eq_destroy
description:
	use this function to destroy the equalizer object
prms:
	handle:[in], equalizer handle
returns:
	none
*/
void eq_destroy(void* handle);


#endif