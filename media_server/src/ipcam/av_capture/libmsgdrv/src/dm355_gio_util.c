/* ===========================================================================
* @file dm355_gio_util.c
*
* @path $(IPNCPATH)/util/
*
* @desc Functions to control dm355 GIO.
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <getopt.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <dm355_gio_util.h>


#ifdef GIO_DEBUG
#define __D(fmt, args...)	fprintf(stderr,"Gio: Debug\n" fmt, ##args)
#else
#define __D(fmt, args...)
#endif

#define __E(fmt, args...)	fprintf(stderr,"Gio: Error\n" fmt, ##args)

#define GIO_MAX_ID	(103)

#define GIO_DEV_PATH	"/proc/gio/"


volatile void *gpio_g3_addr = NULL;
volatile void *gpio_g4_addr = NULL;
volatile void *gpio_g5_addr = NULL;
volatile void *gpio_g6_addr = NULL;

volatile unsigned int *gpio_g3_data_addr = NULL;
volatile unsigned int *gpio_g4_data_addr = NULL;
volatile unsigned int *gpio_g5_data_addr = NULL;
volatile unsigned int *gpio_g6_data_addr = NULL;


/**
* @brief get dm355 gio name
*
*@param *pName pointer to store GIO name
*@param gio_id GIO ID
* @return gio name
*@retval -1 gio id over range
*@retval 0 get gio name success
*/
int dm355_gio_getname( char *pName, int gio_id )
{
	if ((gio_id < 0) || (gio_id > GIO_MAX_ID))
	{
		__E("gio id = %d over range!!\n", gio_id);
		return -1;
	}

	sprintf( pName, GIO_DEV_PATH"gio%d", gio_id );

	//fprintf(stderr, "gio name: %s \n", pName);

	return 0;
}

/**
* @brief read dm355 gio infomation
*
*@param gio_id GIO ID
* @return GIO status
*@retval -1  error
*/
int dm355_gio_read( int gio_id )
{
	int 	fd_gio = 0;
	char	data[5];
	int		result = 0;
	char 	dev_name[25];

	if ((gio_id < 0) || (gio_id > GIO_MAX_ID))
	{
		__E("gio id = %d over range!!\n", gio_id);
		return -1;
	}
	
#if 1 //HI3518
//    printf("GPIO_DATA_ADDR mapped to %p\n", gpio_g4_data_addr);
    switch(gio_id / 10) {
 	  case 3: data[0] = *gpio_g3_data_addr & 0xff; break; 
	  case 4: data[0] = *gpio_g4_data_addr & 0xff; break; 
	  case 5: data[0] = *gpio_g5_data_addr & 0xff; break; 
	  case 6: data[0] = *gpio_g6_data_addr & 0xff; break; 
   	}	
	
    return  data[0] >> (gio_id % 10);
	
#else //DM368
	if( dm355_gio_getname(dev_name,gio_id) < 0 )
	{
		__E("gio get name error!!\n");
		return -1;
	}

	fd_gio = open(dev_name, O_RDWR);

	if( !fd_gio )
	{
		__E("open device error !! gio = %d\n", gio_id);
		return -1;
	}
	result = read(fd_gio,  data, 1);
	if( result <= 0 )
	{
		__E("read device error !! gio = %d\n", gio_id);
		close(fd_gio);
		return -1;
	}
	close(fd_gio);
	if( data[0] == '1')
	{
		return 1;
	}else{
		return 0;
	}

	return -1;
#endif	
}
/**
* @brief set dm355 gio infomation
*
*@param gio_id GIO ID
*@param IsOn GIO setting
* @return result of setting
*@retval -1  error
*@retval 0  success
*/
int dm355_gio_write( int gio_id , int IsOn)
{
	int 	fd_gio = 0;
	char	data[5];
	int		result = 0;
	char 	dev_name[25];
	int		cnt = 0;

#if 1 //HI3518
    unsigned int *gpio_data_addr = NULL;

//    printf("GPIO SETDATAOUTADDR mapped to %p\n", gpio_g4_data_addr);
		
    switch(gio_id / 10)
   	{
   	  case 3: gpio_data_addr = gpio_g3_data_addr; break;
	  case 4: gpio_data_addr = gpio_g4_data_addr; break;
	  case 5: gpio_data_addr = gpio_g5_data_addr; break;
	  case 6: gpio_data_addr = gpio_g6_data_addr; break;
   	}	

    data[0] = 1 << (gio_id % 10);	
    if(IsOn) { *gpio_data_addr |= data[0]; } 
	else { *gpio_data_addr &= (~data[0] & 0xff); } 

	return 0;
#else	
	/* Don't do anything on EVM */
	if( gio_id < 0 || gio_id > GIO_MAX_ID )
	{
		__E("gio id = %d over range!!\n", gio_id);
		return -1;
	}

	if( dm355_gio_getname(dev_name,gio_id) < 0 )
	{
		__E("gio get name error!!\n");
		return -1;
	}

	fd_gio = open(dev_name, O_RDWR);

	if( !fd_gio )
	{
		__E("open device error !! gio = %d\n", gio_id);
		goto GIO_QUIT;
	}

	if( IsOn == 1 )
	{
		for( cnt = 0 ; cnt < sizeof(data) ; cnt ++ )
		{
			data[cnt]='1';
		}
	}
	else if( IsOn == 0 )
	{
		for( cnt = 0 ; cnt < sizeof(data) ; cnt ++ )
		{
			data[cnt]='0';
		}

	}else{
		goto GIO_QUIT;
	}

	result = write(fd_gio,  data, 1);
	if( result <= 0 )
	{
		__E("write device error !! gio = %d\n", gio_id);
		close(fd_gio);
		goto GIO_QUIT;
	}

GIO_QUIT:
	if( fd_gio )
		close(fd_gio);

	return 0;
#endif	
}

#if 0

#define HI_SUCCESS 0
#define DEFAULT_MD_LEN 128
int himm(int argc , char* argv[])
{  
   unsigned int ulAddr = 0;    
   unsigned int ulOld, ulNew;    
   char strNew[16];    
   void* pMem  = NULL;    
   
   if (argc <= 1)    {        printf("usage: %s <address>. sample: %s 0x80040000 \n", argv[0], argv[0]);        EXIT("", -1);    }   
   
   if (argc == 2)    
   {        
 	 if( StrToNumber(argv[1], &ulAddr) == HI_SUCCESS)       
	 { 
	    printf("====dump memory %#lX====\n", ulAddr);            
     #ifdef PC_EMULATOR            
		#define SHAREFILE "../shm"            
		printf("**** is Emulator, use share file : %s ****\n", SHAREFILE);            
		pMem = mmapfile(SHAREFILE , DEFAULT_MD_LEN);            
		if (NULL == pMem)            {                EXIT("Memory Map error.", -1);            }            
	    pMem += ulAddr;           
   	 #else                    
	    pMem = memmap(ulAddr, DEFAULT_MD_LEN);            
	 #endif            
  	    ulOld = *(unsigned int*)pMem;            /*hi_hexdump(STDOUT, pMem, DEFAULT_MD_LEN, 16);*/            
	    printf("%s: 0x%08lX\n", argv[1], ulOld);            
	    printf("NewValue:");            
	    scanf("%s", strNew);            
	    if (StrToNumber(strNew, &ulNew) == HI_SUCCESS) { *(unsigned int*)pMem = ulNew; }            
	    else { printf("Input Error\n"); }        
	 }        
	 else {            printf("Please input address like 0x12345\n");        }   
	}    
	else if (argc == 3)    
	{       
 	  if( StrToNumber(argv[1], &ulAddr) == HI_SUCCESS && StrToNumber(argv[2], &ulNew) == HI_SUCCESS)        
  	  {            
		  pMem = memmap(ulAddr, DEFAULT_MD_LEN);            
		  ulOld = *(unsigned int*)pMem;            /*hi_hexdump(STDOUT, pMem, DEFAULT_MD_LEN, 16);*/            
		  printf("%s: 0x%08lX --> 0x%08lX \n", argv[1], ulOld, ulNew);            
		  *(unsigned int*)pMem = ulNew;        
	   } 
	 }    
     else    {        printf("xxx\n");    }    
    return 0;
	}
#endif

int gpio_init(void){
//    volatile void *gpio_addr = NULL;
    volatile unsigned int *gpio_oe_addr = NULL;
//    volatile unsigned int *gpio_setdataout_addr = NULL;
    volatile unsigned int *gpio_cleardataout_addr = NULL;
    unsigned int reg;
    int fd = open("/dev/mem", O_RDWR);

//    printf("Mapping %X - %X (size: %X)\n", GPIO1_START_ADDR, GPIO1_END_ADDR, GPIO1_SIZE);

    gpio_g3_addr = mmap(0, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_G3_START_ADDR & PAGE_SIZE_MASK);

    gpio_g4_addr = mmap(0, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_G4_START_ADDR & PAGE_SIZE_MASK);

    gpio_g5_addr = mmap(0, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_G5_START_ADDR & PAGE_SIZE_MASK);

	gpio_g6_addr = mmap(0, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_G6_START_ADDR & PAGE_SIZE_MASK);

//    gpio_oe_addr = gpio_addr + GPIO_OE;
      gpio_g3_data_addr = gpio_g3_addr + GPIO_DATA;
      gpio_g4_data_addr = gpio_g4_addr + GPIO_DATA;
      gpio_g5_data_addr = gpio_g5_addr + GPIO_DATA;
      gpio_g6_data_addr = gpio_g6_addr + GPIO_DATA;
//    gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;

    if((gpio_g3_addr == MAP_FAILED) || (gpio_g4_addr == MAP_FAILED) || (gpio_g5_addr == MAP_FAILED) || (gpio_g6_addr == MAP_FAILED)) {
        printf("Unable to map GPIO\n");
        exit(1);
    }
//      printf("GPIO mapped to %p\n", gpio_addr);
//    printf("GPIO OE mapped to %p\n", gpio_oe_addr);
//      printf("GPIO SETDATAOUTADDR mapped to %p\n", gpio_setdataout_addr);
//    printf("GPIO CLEARDATAOUT mapped to %p\n", gpio_cleardataout_addr);

//    reg = *gpio_oe_addr;
//    printf("GPIO1 configuration: %X\n", reg);
//    reg = reg & (0xFFFFFFFF - USR1_LED);
//    *gpio_oe_addr = reg;
//    printf("GPIO1 configuration: %X\n", reg);

//    printf("Start blinking LED USR1\n");
//    int i;
//    for(i=0;i<100;i++)
//    {   
//        printf("ON(%x),%x,%x\n",*gpio_g3_data_addr,*gpio_g4_data_addr,*gpio_g5_data_addr);
//        *gpio_g4_data_addr= 0;//0x4C;

//        sleep(1);
//        printf("OFF(%x,%x,%x)\n",*gpio_g3_data_addr,*gpio_g4_data_addr,*gpio_g5_data_addr);
//        *gpio_g4_data_addr= 0x4C;
//        sleep(1);
//    }

    close(fd);
    return 0;
}
