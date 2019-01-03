#ifndef NOSC_H_
#define NOSC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* overlap of window */
typedef enum
{
	AW_OVERLAP_FIFTY,
	AW_OVERLAP_SEVENTY_FIVE,
}aw_overlap_t;

/* noise type for noise suppression */
typedef enum
{
	AW_STATIONAL,
	AW_LOW_NONSTATIONAL,
	AW_MEDIUM_NONSTATIONAL,
	AW_HIGH_NONSTATIONAL

}aw_sta_t;

/* noise suppression parameters */
typedef struct
{
	/* sampling rate */
	int sampling_rate;
	/*channel num*/
	int channum;
	/* maximum amount of suppression allowed for noise reduction */
	int max_suppression;
	/* overlap of window */
	aw_overlap_t overlap_percent;
	/* stationarity of noise */
	aw_sta_t nonstat;
}aw_ns_prms_t;

/*
	description:
		create and initialise NOSC controller handle
	input:
		prms: parameters
	return value:
		NOSC controller handle
*/
void* NOSCinit(aw_ns_prms_t* prms);
/*
	description
		decreate NOSC controller handle
	input:
		handle: the handle
	return value:
		none
*/
void NOSCdestroy(void* handle);
/*
	description:
		process one frame
	prms:
		   handle: then NOSC handle
		     xout: the signal to process
		framesize: frame size(framesize = byteslen/2)
	return value:
		none
*/
void NOSCdec(void* handle, short* xout, int framesize);


#ifdef __cplusplus
}
#endif

#endif//NOSC_H_