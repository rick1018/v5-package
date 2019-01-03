#!/bin/bash
#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            Hantro Products Oy.                             --
#-                                                                            --
#-                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
#-                            ALL RIGHTS RESERVED                             --
#-                                                                            --
#-                 The entire notice above must be reproduced                 --
#-                  on all copies and should not be removed.                  --
#-                                                                            --
#-------------------------------------------------------------------------------
#-
#--   Abstract     : Common configuration script for the execution of         --
#--                  the SW/HW  verification test cases. To be used on set-up -- 
#--                  time.                                                    --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

#=====---- Common configuration parameters for various sub configurations
PRODUCT="8270"

#SET THIS
#Component versions
export swtag="enc8270_0_62"
export hwtag="enc8270_0_62"
export systag="enc8270_0_62"
export jpegsystag="enc8270_0_62"

#SET THIS
#Test environment information
export kernelversion=/2.6.24-arm2-spnlck/v0_1/
export rootfsversion=v2_3_0_2
export compilerversion=arm-2007q1-21-arm-none-linux-gnueabi-i686-pc-linux-gnu

export testdeviceip=45
export testdeviceversion=v0_1

export comment=""

export SYSTEM_MODEL_HOME="$PWD/$systag/8270_encoder/system/"
export TEST_DATA_HOME="$SYSTEM_MODEL_HOME/$systag"
export TEST_DATA_FILES="$SYSTEM_MODEL_HOME/$systag/test_data_files.txt"


if [ -e random_run ]
then
    TESTDIR=$PWD/random_cases
else
    TESTDIR=$PWD
fi

root_prefix=""
if [ -d "/misc/export" ]
then
    root_prefix="/misc"
fi

#SET_THIS
#Board Configuration
#EB, AXIVERSATILE, VERSATILE
BOARD="VERSATILE"

#SET_THIS
#Test Device IP
TEST_DEVICE_IP="45"

#SET THIS
#User data removal option for sony testing, options y or n
REMOVE_HANTRO_USER_DATA="n"

#SET_THIS
#Compiler Settings
COMPILER_SETTINGS="/afs/hantro.com/i386_linux24/usr/arm/arm-2007q1-21-arm-none-linux-gnueabi-i686-pc-linux-gnu/settings.sh"

#SET_THIS
# Kernel directory for kernel modules compilation
KDIR="/afs/hantro.com/projects/Testing/Board_Version_Control/Realview_PB/PB926EJS/SW/Linux/linux-2.6.28-arm1/v0_1/linux-2.6.28-arm1"

#SET THIS!
# Values: POLLING & IRQ
# DWL implementation to use
DWL_IMPLEMENTATION="IRQ"

if [ "$DWL_IMPLEMENTATION" == "POLLING" ]
then
    IRQ_LINE="-1"
fi

#SET THIS
#To enable internal test cases with extended api. Usually disabled with customer releases
INTERNAL_TEST="y"

#SET_THIS
#Maximum input width resolution
MAX_WIDTH="1920"

#SET_THIS
#Maximum input height resolution
MAX_HEIGHT="1080"

#SET_THIS
#Tracefiles used in case comparison
TRACE_SW_REGISTER="y"
TRACE_EWL="y"

#CHECK_THIS
#System model home directory
SYSTEM_MODEL_HOME="$PWD/$systag/8270_encoder/system"

#CHECK_THIS
#Test case list directory
test_case_list_dir="$SYSTEM_MODEL_HOME/test_data"

#CHECK_THIS
#Test data home
test_data_home=$TEST_DATA_HOME

#CHECK_THIS
#User data home
USER_DATA_HOME=$SYSTEM_MODEL_HOME/test_data

#CHECK_THIS
csv_path="${root_prefix}/export/work/jarsa/8270_0_45/8270_encoder/testdir"

#Endian width
ENDIAN_WIDTH_64_BIT="1"
ENDIAN_WIDTH_32_BIT="0"

# Testcase input data directories
YUV_SEQUENCE_HOME=/afs/hantro.com/sequence/yuv

# Memory base address configuration and interrupt line

if [ "$BOARD" == "EB" ]
then
    HW_BASE_ADDRESS="0x84000000"
    MEM_BASE_ADDRESS="0x08000000"
    MEM_BASE_ADDRESS_END="0x0DFFFFFF"
    
    if [ "$DWL_IMPLEMENTATION" == "IRQ" ]
    then 
        IRQ_LINE="72"
    fi

elif [ "$BOARD" == "AXIVERSATILE" ]
then
    HW_BASE_ADDRESS="0xC4000000"
    MEM_BASE_ADDRESS="0x80000000"
    MEM_BASE_ADDRESS_END="0x85FFFFFF"

elif [ "$BOARD" == "VERSATILE" ]
then
    HW_BASE_ADDRESS="0xC0000000"
    MEM_BASE_ADDRESS="0x02000000"
    MEM_BASE_ADDRESS_END="0x07FFFFFF"
    
    if [ "$DWL_IMPLEMENTATION" == "IRQ" ]
    then 
        IRQ_LINE="30"
    fi    
fi
   	
reporter=$USER
timeform1=`date +%d.%m.20%y`
timeform2=`date +%k:%M:%S`
reporttime=`date +%d%m%y_%k%M | sed -e 's/ //'`




