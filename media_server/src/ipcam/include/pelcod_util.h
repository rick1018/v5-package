/*
 * pelcod_util.h
 */

/* BaudRate Options */
#define BR115200	115200
#define BR57600		57600
#define BR38400		38400
#define BR19200		19200
#define BR9600		9600
#define BR4800		4800
#define BR2400		2400
#define BR1200		1200

/* Databits Options */
#define DATABITS_7	7
#define DATABITS_8	8

/* Parity Options */
#define PARITY_N	0
#define PARITY_O	1
#define PARITY_E	2
#define PARITY_S	3

/* Stopbit Options */
#define STOPBIT_1	1
#define STOPBIT_2	2

#define MIN_CAMERA_ID			0
#define MAX_CAMERA_ID			255
#define DEF_CAMERA_ID			1

#define LOWEST_PT_SPEED		0x01
#define HIGHEST_PT_SPEED	0x3F
#define TURBO_PAN_SPEED		0xFF

#define MIN_PRESET_ID		0x01
#define MAX_PRESET_ID		0x20

#define MIN_ZF_SPEED		0x00
#define MAX_ZF_SPEED		0x03

int RS485_Setup(int baudrate, int databits, int parity, int stopbit);
int Pelco_D_AutoPan(char camera_id);
int Pelco_D_CamLight(char camera_id);
int Pelco_D_ClearPreset(char camera_id, char preset_no);
int Pelco_D_DownLeft(char camera_id, char p_speed, char t_speed);
int Pelco_D_DownRight(char camera_id, char p_speed, char t_speed);
int Pelco_D_FocusFar(char camera_id);
int Pelco_D_FocusNear(char camera_id);
int Pelco_D_GotoPreset(char camera_id, char preset_id);
int Pelco_D_IrisClose(char camera_id);
int Pelco_D_IrisOpen(char camera_id);
int Pelco_D_PanLeft(char camera_id, char p_speed);
int Pelco_D_PanRight(char camera_id, char p_speed);
int Pelco_D_PatternRun(char camera_id);
int Pelco_D_PatternStart(char camera_id);
int Pelco_D_PatternStop(char camera_id);
int Pelco_D_Stop(char camera_id);
int Pelco_D_FocusSpeed(char camera_id, char speed);
int Pelco_D_SetPreset(char camera_id, char preset_no);
int Pelco_D_ZoomSpeed(char camera_id, char speed);
int Pelco_D_TiltDown(char camera_id, char t_speed);
int Pelco_D_TiltUp(char camera_id, char t_speed);
int Pelco_D_UpLeft(char camera_id, char p_speed, char t_speed);
int Pelco_D_UpRight(char camera_id, char p_speed, char t_speed);
int Pelco_D_ZoomIn(char camera_id);
int Pelco_D_ZoomOut(char camera_id);

// end of pelcod_util.h
