#ifndef DRCLOG_H_
#define DRCLOG_H_

/* dynamic range control parameters */
typedef struct
{
	/* sampling rate */
	int sampling_rate;
	/* targer level */
	int target_level;
	/* max gain */
	int max_gain;
	/* min gain */
	int min_gain;
	/* attack time */
	int attack_time;
	/* release time */
	int release_time;
	/* noise threshold */
	int noise_threshold;

}aw_drcLog_prms_t;

/*
	description:
		create and initialise drc controller handle
	input:
		prms: parameters
	return value:
		drc controller handle
*/
void* drcLog_create(aw_drcLog_prms_t* prms);

/*
	description:
		process one frame
	prms:
		   handle: then drc handle
		     xout: the signal to process
		samplenum: sample num in one frame
		      nch: channel num
	return value:
		none
*/
void drcLog_process(void* handle, short* xout,int samplenum, int nch);


/*
	description
		decreate drc controller handle
	input:
		handle: the handle
	return value:
		none
*/
void drcLog_destroy(void* handle);

#endif//DRCLOG_H_
