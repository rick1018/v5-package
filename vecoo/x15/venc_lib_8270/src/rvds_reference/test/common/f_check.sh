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
#--   Abstract     : Script functions for checking the results the SW/HW      --
#--                  verification test cases                                  --
#--                                                                           --
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#-                                                                            --
#- Calculates the execution time for the test case                            --
#-                                                                            --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number			                                      --
#-                                                                            --
#- Exports                                                                    --
#- execution_time                                                             --
#-                                                                            --
#-------------------------------------------------------------------------------

.  ./masterscripts/commonconfig.sh


getExecutionTime()
{
    case_number=$1
    
    execution_time="N/A"
    if [ -e "${TESTDIR}/case_${case_number}.time" ]
    then
        start_time=`grep START ${TESTDIR}/case_${case_number}.time | awk -F : '{print $2}'`
        end_time=`grep END ${TESTDIR}/case_${case_number}.time | awk -F : '{print $2}'`
        let 'execution_time=end_time-start_time'
    fi
    
    export execution_time
}

removeBaseAddress()
{
    ref_case_dir=$1
      
    grep -e "Reg 0x00" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    
    if ( [ "$BOARD" == "EB" ]  || [ "$BOARD" == "AXIVERSATILE" ] )
    then
        grep -e "Reg 0x08" -v $ref_case_dir/sw_reg.trc > foo.tmp
        mv -f foo.tmp $ref_case_dir/sw_reg.trc
    fi
    
    
    grep -e "Reg 0x14" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x18" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x1c" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x20" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x24" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x28" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x2c" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x30" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x34" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x4c" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x58" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x60" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x9c" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0xcc" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0xd0" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0xe" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0xf" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x11" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x12" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x13" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x14" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x15" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x16" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x17" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x100" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x104" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x10c" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    grep -e "Reg 0x108" -v $ref_case_dir/sw_reg.trc > foo.tmp
    mv -f foo.tmp $ref_case_dir/sw_reg.trc
    
}
