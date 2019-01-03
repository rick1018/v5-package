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
#--   Abstract     : Script for compiling software and test benches for       --
#                    versatile                                                --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#
.  ./commonconfig.sh


TARGET=$1

BUILD_LOG=$PWD/"build.log"
rm -rf $BUILD_LOG
if [ -z $TARGET ]
then
    TARGET=versatile
fi

#-------------------------------------------------------------------------------
#-                                                                            --
#- Compiles the encoder test bench.                                           --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : The directory name where test bench make file is located               --
#- 2 : The resulting test bench binary                                        --
#-                                                                            --
#-------------------------------------------------------------------------------
compileEncoderTestBench()
{
    tb=$1
    bin=$2
    echo -n "Compiling ${tb} test bench... "
    if [ -d "../../software/linux_reference/test/${tb}" ]
    then
        cd ../../software/linux_reference/test/${tb}
        make clean >> $BUILD_LOG 2>&1 
        make libclean >> $BUILD_LOG 2>&1 
        make $TARGET >> $BUILD_LOG 2>&1 
        if [ ! -f "$bin" ]
        then
            echo "FAILED"
        else
            echo "OK"
        fi
        cd ../../../../testdir/masterscripts/
    else
        echo "FAILED"
    fi    
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Compiles the memalloc module.                                           --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : The resulting module binary                                        --
#-                                                                            --
#-------------------------------------------------------------------------------

compileMemalloc()
{
    bin=$1
    memalloc_dir="../../software/linux_reference/memalloc/"
    echo -n "Compiling memalloc module... "
    if [ -d "$memalloc_dir" ]
    then
        cd ${memalloc_dir}
        make clean >> $BUILD_LOG 2>&1 
        make >> $BUILD_LOG 2>&1
        if [ ! -f "$bin" ]
        then
            echo "FAILED"
        else
            echo "OK"
        fi
        cd ../../../testdir/masterscripts/
    else
        echo "FAILED"
    fi    
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Compiles ldriver kernel module.                                           --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : The resulting binary                                                   --
#-                                                                            --
#-------------------------------------------------------------------------------

compileLDriver()
{
    bin=$1
    ldriver_dir="../../software/linux_reference/kernel_module/"
    echo -n "Compiling ldriver... "
    if [ -d "$ldriver_dir" ]
    then
        cd ${ldriver_dir}
        make clean >> $BUILD_LOG 2>&1 
        make >> $BUILD_LOG 2>&1 
        if [ ! -f "$bin" ]
        then
            echo "FAILED"
        else
            echo "OK"
        fi
        cd ../../../testdir/masterscripts/
    else
        echo "FAILED"
    fi    
}

compileEncoderSystemModel ()
{
    bin=$1
    sysmodel_dir="../$systag/8270_encoder/system/"
    echo -n "Compiling system model... "
    if [ -d "$sysmodel_dir" ]
    then
        cd ${sysmodel_dir}
        make clean >> $BUILD_LOG 2>&1 
        make testdata >> $BUILD_LOG 2>&1 
        if [ ! -f "$bin" ]
        then
            echo "FAILED"
        else
            echo "OK"
        fi
        cd ../../../masterscripts/
    else
        echo "FAILED"
    fi    
}

# compile all the binaries

echo "Compile target: ${TARGET}"
#echo "source /afs/hantro.com/i386_linux24/usr/arm/arm-2007q1-21-arm-none-linux-gnueabi-i686-pc-linux-gnu/settings.csh"
. "${COMPILER_SETTINGS}"
compileMemalloc "memalloc.ko"
compileLDriver "hx280enc.ko"
compileEncoderTestBench "h264" "h264_testenc"
compileEncoderTestBench "jpeg" "jpeg_testenc"
compileEncoderTestBench "camstab" "videostabtest"
compileEncoderSystemModel "h264_testenc"
