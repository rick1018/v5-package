/* ===========================================================================
* @path $(IPNCPATH)\include
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file dm355_gio_util.h
* @brief Gio API
*/
#ifndef _DM355_GIO_UTIL_H_
#define _DM355_GIO_UTIL_H_

#if defined (__cplusplus)
extern "C" {
#endif

/**
* @ingroup UTILITY API
* @defgroup DM355_GIO_DRV GIO API

* This is the communication interface of GIO manager. 
* @{
*/
/**
* @brief GIO direction.
*/
typedef enum{
	GIO_INPUT, ///< GIO input.
	GIO_OUTPUT ///< GIO output.
}GIO_DIR;
/**
* @brief GIO return value.
*/
typedef enum{
	GIO_ERROR = -1, ///< GIO error.
	GIO_OK ///< GIO ok.
}GIO_RET;

//HI3518 begin
#define PAGE_SIZE 0x1000
#define PAGE_SIZE_MASK 0xfffff000

#define GPIO_G3_START_ADDR 0x201703fc //group 3 data
#define GPIO_G4_START_ADDR 0x201803fc //group 4 data
#define GPIO_G5_START_ADDR 0x201903fc //group 5 data
#define GPIO_G6_START_ADDR 0x201A03fc //group 6 data

//#define GPIO1_SIZE (GPIO1_END_ADDR - GPIO1_START_ADDR) + 1
#define GPIO_SIZE 0x1000
//#define GPIO_END_ADDR GPIO_START_ADDR + GPIO1_SIZE //0x201803fc //0x20140000 //0x201903fc //0x201803fc //direction
//#define GPIO_OE 0x134
#define GPIO_DATA 0x3fc
#define GPIO_DIR  0x400
//#define GPIO_CLEARDATAOUT 0x190

#define FACTORY_DEFAUT 0 //GPIO3_0 	 

#define LAN_LED        2 //GPIO4_2  
#define POWER_LED      3 //GPIO4_3 	 
#define IR_LED         6 //GPIO4_6 	
#define LIGHT_DETECTOR 7 //GPIO4_7 	 

#define IR_CUT         3 //GPIO5_3

#define USR0_LED (1<<21)
#define USR1_LED (1<<22)
#define USR2_LED (1<<23)
#define USR3_LED (1<<24)
//HI3518 end 

int dm355_gio_getname( char *pName, int gio_id ); ///< Get GIO name.
int dm355_gio_read( int gio_id ); ///< Read GIO status.
int dm355_gio_write( int gio_id , int IsOn); ///< Set GIO status.
/** @} */
#if defined (__cplusplus)
}
#endif

#endif

