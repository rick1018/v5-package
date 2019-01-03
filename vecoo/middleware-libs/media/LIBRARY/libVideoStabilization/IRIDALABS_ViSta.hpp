/**
 *  @file IRIDALABS_ViSta.hpp
 *  @brief ViStaEIS API header file.
 *  @copyright <b>Copyright (c) 2008-2017 IRIDA LABS</b>. <br/>
 *   All Rights Reserved IRIDA LABS Proprietary <br/><br/>
 *   All ideas, data and information contained in or disclosed by
 *   this document are confidential and proprietary information of
 *   IRIDA LABS and all rights therein are expressly reserved.
 *   By accepting this material the recipient agrees that this material
 *   and the information contained therein are held in confidence and in
 *   trust and will not be used, copied, reproduced in whole or in part,
 *   nor its contents revealed in any manner to others without the express
 *   written permission of IRIDA LABS.
 */

#ifndef __IRIDALABS_VISTA__HEADERFILE__
#define __IRIDALABS_VISTA__HEADERFILE__

// Required includes for the used 'types' of this file.
#include <arm_neon.h>
#include <stdio.h>


extern "C" {

#ifndef __IRIDALABS__RETINFOTYPE__
	#define __IRIDALABS__RETINFOTYPE__

/** \struct retInfoType
 \brief This structure contains information which is related to 
 error-control.<br/> Most ViSta's API functions are return a pointer to that 
 structure.
 
 \details An example for error checking is given below:<br/>
 <code>
 <br/>retInfoType* retInfo;
 <br/>retInfo = IRIDALABS_ViSta_Initialize(params);
 <br/>if (retInfo->id != 0) {
 <br/>&emsp;printf("Error %i.\n Error Messages:\n%s\n", retInfo->id, 
IRIDALABS_ViSta_Errors());
 <br/>}
 </code>
 
 \var retInfoType::id
 \brief Error id. If no error has been occured then <code>id</code> should be 
always 0. Any other value denotes an <b>error id</b>.
 
 \var int retInfoType::n
 \brief The number of errors recorded from a critical error, back to the API 
function.
 
 \var int retInfoType::ln
 \brief An array that holds the source code lines for error tracking..
*/
	typedef struct retInfoType {
		int id;
		int n;
		int ln[64];
	} retInfoType;
#endif





/** \struct ViStaParamsInitType
\brief This structure contains variables, which are used during ViSta's 
initializaton. These variables can change in different API versions depending 
the application.

\var ViStaParamsInitType::frame_width
\brief The frame's width for the video sequence that it's going to be 
stabilized.

\var ViStaParamsInitType::frame_height
\brief The frame's height for the video sequence that it's going to be 
stabilized.

\var ViStaParamsInitType::frame_width_stride
\brief The frame's width's stride for the video sequence that it's going to 
be stabilized.

\var ViStaParamsInitType::frame_height_stride
\brief The frame's height's stride for the video sequence that it's going to 
be stabilized. I.e. a frame may have height 1080 but height stride 1088.

\var ViStaParamsInitType::max_yaw_degrees
\brief Denotes the maximum yaw correction in degrees.

\var ViStaParamsInitType::max_pitch_degrees
\brief Denotes the maximum pitch correction in degrees.

\var ViStaParamsInitType::max_roll_degrees
\brief Denotes maximum roll correction in degrees.

\var ViStaParamsInitType::output_scale
\brief Scaling for angle loss reduction. Must be > 1.0 and < 1.5.

\var ViStaParamsInitType::gyro_frequency_in_Hz
\brief The gyro frequency of the system in Hz

\var ViStaParamsInitType::target_fps
\brief The targeted fps for the system (i.e. 30).

\var ViStaParamsInitType::number_of_input_buffers
\brief Number of input buffers where frames can be delayed in order to 
syncronized with correct stabilization. <b>Note:</b> The number of input 
buffers is actually defined by ViSta's internal implementation. If the given 
number is not correct for a version of ViSta, then ViSta will produce an 
error during initialization, and will propose the correct number of input 
buffers to be used. In this way the Client will always be aware about the 
input framebuffer length and it's relative video delay.

\var ViStaParamsInitType::number_of_output_buffers
\brief Number of output buffers where stabilized results can be stored (ie 
for parellel ViSta stabilize & system encoding).

\var ViStaParamsInitType::operation_mode
\brief ViSta Library may contain multiple stabilization implementations for 
different operation modes. This value is used to select specific 
implementation.

\var ViStaParamsInitType::render_quality
\brief Operation modes may be able to render output frames with different 
levels of rendering quality. A value of '0' is referring to fastest processing
with lower quality. Higher values means better quality for higher processing.
*/
typedef struct ViStaParamsInitType {
	int frame_width;
	int frame_height;
	int frame_width_stride;
	int frame_height_stride;
	float max_yaw_degrees;
	float max_pitch_degrees;
	float max_roll_degrees;
	float output_scale;
	float gyro_frequency_in_Hz;
	float target_fps;
	int number_of_input_buffers;
	int number_of_output_buffers;
	int operation_mode;
	int render_quality;
} ViStaParamsInitType;





/** \struct ViStaFrame
\brief This structure contains all data that are related with each Video 
Frame that is being processed. It can change depending the application. In 
current version it refers to YUV 4:2:0 semi-interleaved color format video 
frames, which exist in memory.

\var ViStaFrame::pY
\brief Memory Address which contains the Y-Data of the video frame.

\var ViStaFrame::pUV
\brief Memory Address which contains the UV-Data of the video frame.

\var ViStaFrame::FrameID
\brief A unique FrameID number for each frame. This is used for Gyro-Data 
syncrhonization.

\var ViStaFrame::Exposure
\brief The exposure time used, for the specific frame in milli-seconds.

\var ViStaFrame::ZoomFactor
\brief The Zoom factor that is selected by the user for the specific frame 
(multiplied by 10000).

\var ViStaFrame::flag
\brief This number is set by ViSta on output, denoting some extra information 
about the processed frame. Values are:<br/>
	0: Frame was not processed, and should be rejected (i.e. initialization 
delay)<br/>
	1: Frame processed correctly.

\var ViStaFrame::mptz
\brief timestamp value.
*/
typedef struct ViStaFrame {
	uint8_t* __restrict pY;
	uint8_t* __restrict pUV;
	size_t FrameID;
	uint32_t Exposure;
	int ZoomFactor;
	int flag;
	uint64_t mptz;
	uint8_t Orientation;
} ViStaFrame;



/** \struct ViStaFrameMemory
\brief Depending the data transfer model that is going to be used, ViSta 
should be able to know how many memory frame buffers are available for Input 
and Output. ViSta may allocate those buffers internally and share them to 
the client, or may use client's allocate buffers.

\var ViStaFrameMemory::Number_of_Input_Mems
\brief Number of input Frame Buffers (This is defined at ViStaParamsInitType 
during ViSta's Initialization)

\var ViStaFrameMemory::Number_of_Output_Mems
\brief Number of output Frame Buffers (This is defined at ViStaParamsInitType 
during ViSta's Initialization)

\var ViStaFrameMemory::inp_pY
\brief An array with the pointers to Y-Channel for every Input buffer.

\var ViStaFrameMemory::inp_pUV
\brief An array with the pointers to UV-Channels for every Input buffer.

\var ViStaFrameMemory::out_pY
\brief An array with the pointers to Y-Channel for every Output buffer.

\var ViStaFrameMemory::out_pUV
\brief An array with the pointers to UV-Channels for every Output buffer.

\var ViStaFrameMemory::Y_channel_bytes
\brief The allocated bytes for Y-Channels

\var ViStaFrameMemory::UV_channels_bytes
\brief The allocated bytes for UV-Channels
*/
typedef struct ViStaFrameMemory {
	uint32_t Number_of_Input_Mems;
	uint32_t Number_of_Output_Mems;
	uint8_t **inp_pY;
	uint8_t **inp_pUV;
	uint8_t **out_pY;
	uint8_t **out_pUV;
	uint32_t Y_channel_bytes;
	uint32_t UV_channels_bytes;
} ViStaFrameMemory;



/**
\brief This is the main initialization function for ViSta. It must be called 
when ViSta is going to be used for first time, or after ViSta's 
IRIDALABS_ViSta_Shutdown() function has been called.
\details This function should be called first when ViSta is going to be 
used. It initialize internal ViSta's modules, allocates relative memory and 
buffers and may acquire other resources (i.e. devices of the system).<br/>
This function also initialize ViSta for specific Video parameters (Width 
/ Height / FPS / Gyro Frequency etc). This means that if Video parameters 
needs to change, then this function has to be called again.<br/>
Calling this function also means that ViSta has not been initialized before. 
DeInitialization of ViSta is done via the IRIDALABS_ViSta_Shutdown() function.

\param [in] pInitParams
\brief Takes a pointer to ViStaParamsInitType structure, with all necessary 
initialization data.

\return Pointer to retInfoType struct for error checking.
*/
retInfoType* IRIDALABS_ViSta_Initialize(
	ViStaParamsInitType* pInitParams
);

/**
\brief Use this function to update ViStaEIS with the allocated I/O Frame 
Buffers.

\param [in] pFrameMem Pointer to a pre-initialized ViStaFrameMemory structure 
which will contain the relative information.

\return Pointer to retInfoType struct for error checking.
*/
retInfoType* IRIDALABS_ViSta_MemorySetup(
	ViStaFrameMemory* pFrameMem
);

/**
\brief This function, should be called to create a new video 
stabilization session, which will have predefined ViSta initialization 
parameters (i.e. same Frame Size, FPS etc) according to the ViSta 
Initialization phase.<br/>
This function is actually does a fast-initialization, thus video 
stabilization to start almost immediately without the significant delayes 
that ViSta_Initialize() function includes.<br/>
If the clients needs to record a new video with the parameters set at 
initialization stage, then he should use this function. If the client needs 
to change the video paramaters (i.e. frame size, FPS etc) then ViSta has to 
be shutdown first using IRIDALABS_ViSta_Shutdown() and reinitialized with new 
parameters using the IRIDALABS_ViSta_Initialize() function.<br/>
<br/>
This function takes as arguments two references to ViStaFrame structures, 
which are filled with the Frame memory pointers which client should use to 
write/read the next Frame (i.e. YUV) data into/from ViSta.<br/>
More specifically when this function is called, client should use the 
pointers of Y and UV channels that exist at FrameInp.pY and FrameInp.pUV 
variables in order to set the next input frame for processing to ViSta. When 
ViSta is called to process the next frame later on, it will use as input 
frame data, the data that will exist at the memory address of the above two 
pointers (FrameInp.pY and FrameInp.pUV).

\param [out] FrameInp Filled up a ViStaFrame structure with the Y and UV 
pointers where the next frame's data should be placed.

\param [out] FrameOut: This is usually ignored, but may be need in some 
implementations. It is filled up a ViStaFrame structure with the Y and UV 
pointers where the next frame's stabilization data will be exist. 

\return Pointer to retInfoType struct for error checking.
*/
retInfoType* IRIDALABS_ViSta_SessionStart(
	ViStaFrame *FrameInp,
	ViStaFrame *FrameOut
);

/**
\brief This function, should be called when a video stabilization 
sessions ends (i.e. when the user is pressing the [stop record] button).<br/>
This function notify the ViSta algorithm that the client won't provide 
anymore input data, and depending the 'end_status' variable ViSta will 
either stop the processing, or it will continue to produce delayed output 
frames till almost all input frames has been exported to output.

\param [inout] end_status <br/>
In: Value 0 means stop immediately ViSta.<br/>
In: <b>Note: This may not be supported</b>: Value 1 means ViSta will continue 
to export frames till the end.<br/>
&emsp;Out: The value of end_status, when it's input value is 1, corresponds to 
the number of Frames that are waiting to be exported before the ViSta 
session is actually ends.

\return Pointer to retInfoType struct for error checking.
*/
retInfoType* IRIDALABS_ViSta_SessionEnd(uint32_t *end_status);


/**
\brief This function is used for two reasons. To provide ViStaEIS with the 
next recorded frame that must be stabilized, and provide to the client the 
memory address where the next frame must be recorded.

@param [inout] FrameInp Should contain the information of a new recorded 
frame that ViStaEIS must stabilize. When the function returns, it contains 
the memory addresses (Y/UV) where next frame should be written.

\return Pointer to retInfoType struct for error checking.
 */
retInfoType* IRIDALABS_ViSta_setNextInputFrame(ViStaFrame *FrameInp);

/**
\brief This function informs ViStaEIS where to write the next stabilized 
frame. The writting procedure will take place when 
IRIDALABS_ViSta_ProcessFrame() is called.

@param [in] FrameOut Should contain the memory addresses (Y/UV) where 
ViStaEIS will write the next frame.

\return Pointer to retInfoType struct for error checking.
 */
retInfoType* IRIDALABS_ViSta_setNextOutputFrame(ViStaFrame *FrameOut);

/**
\brief This function utilizes the FrameInp data that have provided via the 
IRIDALABS_ViSta_setNextInputFrame() function before, to stabilize the video 
and produce the results at the FrameOut data that has been set from a 
IRIDALABS_ViSta_setNextOutputFrame() call before.

\return Pointer to retInfoType struct for error checking.
*/
retInfoType* IRIDALABS_ViSta_ProcessFrame();

/**
\brief This function is used to update ViSta buffers with new gyroscopic data
\details For every video frame that is inserted to ViSta via 
IRIDALABS_ViSta_ProcessFrame() for stabilization, there are corresponding 
gyroscopic data. Those gyro-data must have been inserted to ViSta using 
this (IRIDALABS_ViSta_InsertGyroData()) function before the call to 
IRIDALABS_ViSta_ProcessFrame().<br/>
<br/>
As gyroscopic data are recorded in three dimensions (yaw, pitch, roll), the 
velocity of the camera in the space is characterized by three numbers in each 
(sampled) time. For this reason a recording of the velocity of the camera in 
space is characterized by one <b>gyro-triplet = {yaw,pitch,roll}</b> per 
sampled time.<br/>
Depending the gyro frequency and the exposure of the frame that is been 
recorded, more than one gyro-triplets are recorded during a frame capture. 
Those gyro-triplets that are recorded between the start of a capture of one 
frame with FrameID = t, and the start of the capture of the next frame with 
FrameID = t+1, are corresponding to the frame with FrameID = t, and should be 
characterized with a GyroID equal to the FrameID; GyroID = FrameID = t.<br/>
For video recording at 30 FPS the time between the two frames with IDs t and 
t+1 should be steady at 1/30 msec.<br/>
All gyro-triplets that have same GyroID number, are referring to one Frame, 
and forms a <b>gyro-packet</b>. Thus each <i>gyro-packet</i> is described 
by:<br/>
<ol>
	<li><b>Gyro_ID</b>: A unique number that corresponds to a recorded frame 
period of time.</li>
	<li><b>Number_of_Gyro_Triplets</b>: A number of gyro-triplets that were 
recorded during that recorded frame period of time.</li>
	<li><b>Gyro_xyz</b>: An array of consecutive floating values, where 
every 3 values on a row, represents a gyro-triplet.</li>
</ol>
<br/>
This IRIDALABS_ViSta_InsertGyroData() function is used to insert 
<i>gyro-packets</i> into ViSta.
<br/>
Because gyro-packets may be available while ViSta is processing frames, this 
function is designed to be able to run "in-parallel" with ViSta, ie. on 
another thread, in order not to be blocked for significant time by ViSta.

\param [in] Number_of_Gyro_Triplets The number of recorded 
<i>gyro-triplets</i> of a <i>gyro-packet</i>.

\param [in] Gyro_xyz A pointer to float variables of the gyro-triplets of the 
gyro-packet. I.e. if the [Number_of_Gyro_Triplets] is equal to 10 then the 
[Gyro_xyz] pointer should point to a 10*3 = 30 floating point elements array.

\param [in] Gyro_ID The gyro ID of the gyro-packet which must be equal to the 
FrameID of the frame which was captured while the gyro-triplets of this 
gyro-packet was record.

\param [in] z_time_usec In low-frequency gyroscopes, it must be also known 
the  time difference from of the first gyro-triplet of a gyro-session to the 
start of the recording of the corresponding frame (zero_time).

\return Pointer to retInfoType struct for error checking.
*/
retInfoType* IRIDALABS_ViSta_InsertGyroData(
	uint32_t Number_of_Gyro_Triplets,
	float* Gyro_xyz,
	size_t Gyro_ID,
	uint32_t z_time_usec,
	uint64_t* Gyro_mptz
);

/**
\brief Shutdown ViSta's engine and clean up internal memory and variables. It 
also sets ViSta to Uninitialized status, thus to be able to initialized again 
(i.e. with new video parameters).

\return Pointer to retInfoType struct for error checking.
*/
retInfoType* IRIDALABS_ViSta_Shutdown(void);

/**
\brief ViSta has internally it's own error handling mechanism based on error 
ID numbers and meaningfull strings. When an error is captured, all the 
relative information required to recognize the error is recorded in the error 
handling mechanism.<br/>
This function exports in human readable form the recorded errors that where 
produced.

\return Pointer to string array which holds error logs. Terminates with '\0'.
*/
char* IRIDALABS_ViSta_Errors();

/**
\brief Returns a string with the Build version of the Library.
\return The Library version as string.
 */
const char* IRIDALABS_ViSta_GetVersion();

} // extern "C"
#endif
