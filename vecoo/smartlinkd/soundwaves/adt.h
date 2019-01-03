#ifndef _ADT_H_
#define _ADT_H_


/* frequency type selector */
typedef enum
{
	/*Low frequency, 2K~5K */
	FREQ_TYPE_LOW =0,
	/*Medium frequency, 8K~12K*/
	FREQ_TYPE_MIDDLE,
	/*High frequency£¬ 16K~20K*/
	FREQ_TYPE_HIGH

}freq_type_t;

/*macros of return valule of decoder*/
/* Decoding error */
#define RET_DEC_ERROR -1
/* Decode returns normal */
#define RET_DEC_NORMAL 0
/* The decoding is not finished and the decoding result can not be obtained */
#define RET_DEC_NOTREADY 1
/* Decoding finished */
#define RET_DEC_END 2


/* definition of decoder config paramters */
typedef struct
{
	/* Supported maximum string length (bytes) */
	int max_strlen;
	/* Sampling Rate */
	int sample_rate;
	/* Frequency selection range */
	freq_type_t freq_type;
	/* The number of bytes transmitted per packet */
	int group_symbol_num;
	/* Whether to use error correction code */
	int error_correct;
	/* The ability of error correction code to correct error */
	int error_correct_num;
}config_decoder_t;

/*
	Description: Create a decoder
	Parameters: decode_config: parameter structure (pointer)
	Return Value: Decoder handle, NULL Indicates that the creation failed
*/
void* decoder_create(config_decoder_t* decode_config );

/*
	Description: Reset the decoder
	Parameters: handle: Decoder handle
	Return Value: None
*/
void decoder_reset(void* handle);
/*
	Description: Get the amount of data per frame (number of samples)
	Parameters: handle: Decoder handle
	Return Value: The amount of data per frame (number of samples, 16 bits per sample)
*/
int decoder_getbsize(void* handle);
/*
	Description: Fill the data with the decoder
	Parameters: handle: Decoder handle
				Pcm: data buffer, to ensure that the sample points are equal to the return value of decoder_getbsize
	Return Value: A macro definition with the decoded return value
*/
int decoder_fedpcm(void* handle, short* pcm);
/*
	Description: Get the decoding result
	Parameters: handle: Decoder handle
	Str: decoding result buffer
	Return Value: A macro definition with the decoded return value
*/
int decoder_getstr(void* handle, unsigned char* str);
/*
	Description: Releases the decoder handle
	Parameters: handle: Decoder handle
	Return Value: None
*/
void decoder_destroy(void* handle);

/* definition of encoder config paramters */
typedef struct
{
	/* Supported maximum string length (bytes) */
	int max_strlen;
	/* Sampling frequency */
	int sample_rate;
	/* Frequency range selection */
	freq_type_t freq_type;
	/* The number of bytes transmitted per packet */
	int group_symbol_num;
	/* Whether to use error correction coding */
	int error_correct;
	/* Error correction capability (number of bytes)*/
	int error_correct_num;
}config_encoder_t;

/* macros of return value of encoder */
/* Coding error */
#define		RET_ENC_NORMAL 0
/* Coding end */
#define		RET_ENC_END 1
/* Coding error */
#define		RET_ENC_ERROR -1

/*
	Description: Release the encoder handle
	Parameters: handle: Encoder handle
	Return Value: None
*/
void encoder_destroy(void* handle);
/*
	Description: Creates an encoder handle
	Parameters: config: parameter structure (pointer)
	Return Value: Encoder handle, NULL indicates that the creation failed
*/
void* encoder_create(config_encoder_t* config);
/*
	Description: Reset the encoder
	Parameters: handle: Encoder handle
	Return Value: None
*/
void encoder_reset(void* handle);
/*
	Description: Get the frame buffer size (in bytes) for the external allocation of the frame buffer buffer
	Parameters: handle: Encoder handle
	Return Value: Frame Cache Size (Bytes)
*/
int encoder_getoutsize(void* handle);
/*
	Description: Get frame data
	Parameters: handle: Encoder handle
				Outpcm: frame data buffer (external allocation)
	Return Value: A macro definition with the return value of the encoder
*/
int encoder_getpcm(void* handle, short* outpcm);
/*
	Description: Sets the string to be encoded, and the string ends with the empty character '\ 0'
	Parameters: handle: Encoder handle
				Input: A string that ends with a null character
	Returns the macro definition with the return value of the encoder
*/
int encoder_setinput(void* handle, unsigned char* input);

#endif
