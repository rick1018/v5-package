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
#--   Abstract     : Script for setting up configuration(s).                  --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

. ./commonconfig.sh

# set up compile time flags

#Hantro user data removal
if [ "$REMOVE_HANTRO_USER_DATA" == "y" ]
then
    sed "s,CONFFLAGS += -DMPEG4_HW_VLC_MODE_ENABLED -DMPEG4_HW_RLC_MODE_ENABLED #-DREMOVE_HANTRO_USER_DATA.*,CONFFLAGS += -DMPEG4_HW_VLC_MODE_ENABLED -DMPEG4_HW_RLC_MODE_ENABLED -DREMOVE_HANTRO_USER_DATA," ../../software/linux_reference/Makefile > tmp
    mv tmp ../../software/linux_reference/Makefile

elif [ "$REMOVE_HANTRO_USER_DATA" == "n" ]
then    
    sed "s,CONFFLAGS += -DMPEG4_HW_VLC_MODE_ENABLED -DMPEG4_HW_RLC_MODE_ENABLED -DREMOVE_HANTRO_USER_DATA.*,CONFFLAGS += -DMPEG4_HW_VLC_MODE_ENABLED -DMPEG4_HW_RLC_MODE_ENABLED #-DREMOVE_HANTRO_USER_DATA," ../../software/linux_reference/Makefile > tmp
    mv tmp ../../software/linux_reference/Makefile
fi

# HW base address
sed "s/#define[ \t]*INTEGRATOR_LOGIC_MODULE0_BASE[ \t].*/#define INTEGRATOR_LOGIC_MODULE0_BASE $HW_BASE_ADDRESS/" ../../software/linux_reference/kernel_module/hx280enc.c > tmp
mv tmp ../../software/linux_reference/kernel_module/hx280enc.c

# Memory base address
sed "s/HLINA_START_ADDRESS= .*/HLINA_START_ADDRESS=$MEM_BASE_ADDRESS/" ../../software/linux_reference/memalloc/Makefile > tmp
mv tmp ../../software/linux_reference/memalloc/Makefile

sed "s/HLINA_END_ADDRESS=.*/HLINA_END_ADDRESS=$MEM_BASE_ADDRESS_END/" ../../software/linux_reference/memalloc/Makefile > tmp
mv tmp ../../software/linux_reference/memalloc/Makefile


# Kernel directory for kernel modules compilation
sed "s,KDIR[ \t]*:=.*,KDIR := $KDIR," ../../software/linux_reference/memalloc/Makefile > tmp
mv tmp ../../software/linux_reference/memalloc/Makefile

sed "s,KDIR[ \t]*:=.*,KDIR := $KDIR," ../../software/linux_reference/kernel_module/Makefile > tmp
mv tmp ../../software/linux_reference/kernel_module/Makefile

#Interrupt or polling mode
if [ "$DWL_IMPLEMENTATION" == "IRQ" ]
then
    sed "s/POLLING[ \t]*=.*/POLLING = n/" ../../software/linux_reference/Makefile > tmp
    mv tmp ../../software/linux_reference/Makefile

elif [ "$DWL_IMPLEMENTATION" == "POLLING" ]
then
    sed "s/POLLING[ \t]*=.*/POLLING = y/" ../../software/linux_reference/Makefile > tmp
    mv tmp ../../software/linux_reference/Makefile
fi

# IRQ line
sed "s/#define[ \t]*VP_PB_INT_LT[ \t].*/#define VP_PB_INT_LT                    $IRQ_LINE/" ../../software/linux_reference/kernel_module/hx280enc.c > tmp
mv tmp ../../software/linux_reference/kernel_module/hx280enc.c

#Endian width
if ( [ "$BOARD" == "EB" ]  || [ "$BOARD" == "AXIVERSATILE" ] )
then

    sed "s/#define[ \t]*ENC8270_INPUT_SWAP_32_YUV[ \t].*/#define ENC8270_INPUT_SWAP_32_YUV                         $ENDIAN_WIDTH_64_BIT/" ../../software/source/common/enccfg.h > tmp
    mv tmp ../../software/source/common/enccfg.h

    sed "s/#define[ \t]*ENC8270_INPUT_SWAP_32_RGB16[ \t].*/#define ENC8270_INPUT_SWAP_32_RGB16                       $ENDIAN_WIDTH_64_BIT/" ../../software/source/common/enccfg.h > tmp
    mv tmp ../../software/source/common/enccfg.h
    
    sed "s/#define[ \t]*ENC8270_INPUT_SWAP_32_RGB32[ \t].*/#define ENC8270_INPUT_SWAP_32_RGB32                        $ENDIAN_WIDTH_64_BIT/" ../../software/source/common/enccfg.h > tmp
    mv tmp ../../software/source/common/enccfg.h

    sed "s/#define[ \t]*ENC8270_OUTPUT_SWAP_32[ \t].*/#define ENC8270_OUTPUT_SWAP_32                        $ENDIAN_WIDTH_64_BIT/" ../../software/source/common/enccfg.h > tmp
    mv tmp ../../software/source/common/enccfg.h
    
    sed "s/#define[ \t]*VS8270_INPUT_SWAP_32_YUV[ \t].*/#define VS8270_INPUT_SWAP_32_YUV                        $ENDIAN_WIDTH_64_BIT/" ../../software/source/camstab/vidstabcfg.h > tmp
    mv tmp ../../software/source/camstab/vidstabcfg.h

    sed "s/#define[ \t]*VS8270_INPUT_SWAP_32_RGB16[ \t].*/#define VS8270_INPUT_SWAP_32_RGB16                        $ENDIAN_WIDTH_64_BIT/" ../../software/source/camstab/vidstabcfg.h > tmp
    mv tmp ../../software/source/camstab/vidstabcfg.h
    
    sed "s/#define[ \t]*VS8270_INPUT_SWAP_32_RGB32[ \t].*/#define VS8270_INPUT_SWAP_32_RGB32                        $ENDIAN_WIDTH_64_BIT/" ../../software/source/camstab/vidstabcfg.h > tmp
    mv tmp ../../software/source/camstab/vidstabcfg.h

elif [ "$BOARD" == "VERSATILE" ]
then

    sed "s/#define[ \t]*ENC8270_INPUT_SWAP_32_YUV[ \t].*/#define ENC8270_INPUT_SWAP_32_YUV                         $ENDIAN_WIDTH_32_BIT/" ../../software/source/common/enccfg.h > tmp
    mv tmp ../../software/source/common/enccfg.h

    sed "s/#define[ \t]*ENC8270_INPUT_SWAP_32_RGB16[ \t].*/#define ENC8270_INPUT_SWAP_32_RGB16                       $ENDIAN_WIDTH_32_BIT/" ../../software/source/common/enccfg.h > tmp
    mv tmp ../../software/source/common/enccfg.h
    
    sed "s/#define[ \t]*ENC8270_INPUT_SWAP_32_RGB32[ \t].*/#define ENC8270_INPUT_SWAP_32_RGB32                        $ENDIAN_WIDTH_32_BIT/" ../../software/source/common/enccfg.h > tmp
    mv tmp ../../software/source/common/enccfg.h

    sed "s/#define[ \t]*ENC8270_OUTPUT_SWAP_32[ \t].*/#define ENC8270_OUTPUT_SWAP_32                        $ENDIAN_WIDTH_32_BIT/" ../../software/source/common/enccfg.h > tmp
    mv tmp ../../software/source/common/enccfg.h
    
    sed "s/#define[ \t]*VS8270_INPUT_SWAP_32_YUV[ \t].*/#define VS8270_INPUT_SWAP_32_YUV                        $ENDIAN_WIDTH_32_BIT/" ../../software/source/camstab/vidstabcfg.h > tmp
    mv tmp ../../software/source/camstab/vidstabcfg.h

    sed "s/#define[ \t]*VS8270_INPUT_SWAP_32_RGB16[ \t].*/#define VS8270_INPUT_SWAP_32_RGB16                        $ENDIAN_WIDTH_32_BIT/" ../../software/source/camstab/vidstabcfg.h > tmp
    mv tmp ../../software/source/camstab/vidstabcfg.h
    
    sed "s/#define[ \t]*VS8270_INPUT_SWAP_32_RGB32[ \t].*/#define VS8270_INPUT_SWAP_32_RGB32                        $ENDIAN_WIDTH_32_BIT/" ../../software/source/camstab/vidstabcfg.h > tmp
    mv tmp ../../software/source/camstab/vidstabcfg.h
    
fi

#Setup Internal test for H264 & Mpeg4

if ( [ "$INTERNAL_TEST" == "y" ] )
then
    sed "s/INTERNAL_TEST[ \t]*=.*/INTERNAL_TEST = y/" ../../software/linux_reference/test/h264/Makefile  > tmp
    mv tmp ../../software/linux_reference/test/h264/Makefile

elif ( [ "$INTERNAL_TEST" == "n" ] )
then
    sed "s/INTERNAL_TEST[ \t]*=.*/INTERNAL_TEST = n/" ../../software/linux_reference/test/h264/Makefile  > tmp
    mv tmp ../../software/linux_reference/test/h264/Makefile

fi

if [ ! -d "../$systag" ]
then

	 mkdir ../$systag
	 cd ../$systag

	 git clone -n /afs/hantro.com/projects/8270/git/8270_encoder/

	     if [ -d 8270_encoder ]
	     then
        	 cd 8270_encoder/
	     else
        	 echo "System model tag 1 is not correct!"
		 exit
	     fi

	 git checkout $systag
	 git checkout -b branch_$systag
	 cd system

	 #make clean >> /dev/null 2>&1
	 #make testdata >> /dev/null 2>&1

	 mkdir $systag

	 echo "video_stab_result.log" >> $PWD/$systag/test_data_files.txt
	 cd ../../../masterscripts
	 
fi

if ( [ "$TRACE_SW_REGISTER" == "y" ] )
then
    sed "s,#DEBFLAGS += -DTRACE_REGS.,DEBFLAGS += -DTRACE_REGS," ../../software/linux_reference/Makefile  > tmp
    mv tmp ../../software/linux_reference/Makefile
    sed "s,#DEBFLAGS += -DTRACE_REGS.,DEBFLAGS += -DTRACE_REGS," ../${systag}/8270_encoder/software/linux_reference/Makefile  > tmp
    mv tmp ../${systag}/8270_encoder/software/linux_reference/Makefile

elif ( [ "$TRACE_SW_REGISTER" == "n" ] )
then
    sed "s,.DEBFLAGS += -DTRACE_REGS,#DEBFLAGS += -DTRACE_REGS," ../../software/linux_reference/Makefile  > tmp
    mv tmp ../../software/linux_reference/Makefile
    sed "s,.DEBFLAGS += -DTRACE_REGS,#DEBFLAGS += -DTRACE_REGS," ../${systag}/8270_encoder/software/linux_reference/Makefile  > tmp
    mv tmp ../${systag}/8270_encoder/software/linux_reference/Makefile

fi

if ( [ "$TRACE_EWL" == "y" ] )
then
    sed "s,#DEBFLAGS += -DTRACE_EWL.,DEBFLAGS += -DTRACE_EWL," ../../software/linux_reference/Makefile  > tmp
    mv tmp ../../software/linux_reference/Makefile

elif ( [ "$TRACE_EWL" == "n" ] )
then
    sed "s,.DEBFLAGS += -DTRACE_EWL,#DEBFLAGS += -DTRACE_EWL," ../../software/linux_reference/Makefile  > tmp
    mv tmp ../../software/linux_reference/Makefile

fi


