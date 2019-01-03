#!/bin/sh    


#Imports
. ./f_check.sh

# import commonconfig
if [ ! -e masterscripts/commonconfig.sh ]
then
    echo "<commonconfig.sh> missing"
    exit 1
else
.  ./masterscripts/commonconfig.sh
fi

supportedPicWidth=$MAX_WIDTH
supportedPicHeight=$MAX_HEIGHT
roi=0
sei=0
    
let "case_nro=$1"
DECODER=./hx170dec_pclinux

# Test data dir
case_dir=$test_data_home/case_$case_nro

# If test data doesn't exist, generate it
if [ ! -e $case_dir ]
then

    cd $systag/8270_encoder/system
    ./test_data/test_data.sh $case_nro >> /dev/null 2>&1
    sleep 1
    cd ../../../

fi

if [ "$test_case_list_dir" == "" ]
then
    echo "Check test_case_list_dir missing from <commonconfig.sh>."
    exit -1
fi

if [ ! -e $test_case_list_dir/test_data_parameter_h264.sh ]
then
    echo "<test_data_parameter_h264.sh> doesn't exist!"
    exit -1
fi

. $test_case_list_dir/test_data_parameter_h264.sh "$case_nro"
set_nro=$((case_nro-case_nro/5*5))          
picWidth=$((width[set_nro]))
picHeight=$((height[set_nro]))
casetestId=$((testId[set_nro]))

if [ $picWidth -gt $supportedPicWidth ] || [ $picHeight -gt $supportedPicHeight ]
then
    echo "H264 Case $case_nro Unsupported picture width or height!"
    exit 2
fi

if ( [ "$INTERNAL_TEST" == "n" ] )
then

    if [ $casetestId != "0" ] 
    then
        echo "Internal test, does not work with customer releases!"
        exit 2
    fi
fi

# Find the failing picture
findFailingPicture()
{

    if ! [ -e $1 ] || ! [ -e $2  ]
    then
        echo "Missing decoded YUV file(s) to cmp!"
        failing_picture=0
        export failing_picture
        return 1
    fi

    # Find the failing picture
    # Calculate the failing picture using the picture width and height
    error_pixel_number=`cmp $1 $2 | awk '{print $5}' | awk 'BEGIN {FS=","} {print $1}'`

    if [ "$error_pixel_number" != "" ] 
    then
	# The multiplier should be 1,5 (multiply the values by 10)
	pixel_in_pic=$((picWidth*picHeight*3/2))
	failing_picture=$((error_pixel_number/pixel_in_pic))
	export failing_picture
    return 1
    else
    return 0
    fi
}

if [ -e ${case_dir}/stream.h264 ]
then
(
    echo -e -n "H.264 Case $case_nro\t"
    
    #echo "$TESTDIR"
    
    while [ ! -e ${TESTDIR}/case_${case_nro}/run_${case_nro}_done ]
    do

        sleep 1 
	#if [ -e run_done ]
	#then
	#    exit 1
	#fi	   
    done

    
    removeBaseAddress "$TESTDIR/case_$case_nro"
    removeBaseAddress "$case_dir"

    # Compare output to reference
    if (cmp -s ${TESTDIR}/case_${case_nro}/case_$case_nro.h264 ${case_dir}/stream.h264)
    then
    
   
            if ( [ "$TRACE_SW_REGISTER" == "y" ] )
	    then
	        if (cmp -s ${TESTDIR}/case_${case_nro}/sw_reg.trc ${case_dir}/sw_reg.trc)
                then
                    echo "STREAM OK, SW REGS TRACE OK"
	            exit 0
                else
                    echo "STREAM OK, SW REGS TRACE FAILED"
	            exit -1
                fi
	    fi 
     
        echo "OK"
        exit 0
    else
    
        if ( [ "$TRACE_SW_REGISTER" == "y" ] )
        then
    
            
	    if (cmp -s ${TESTDIR}/case_${case_nro}/sw_reg.trc ${case_dir}/sw_reg.trc)
            then
                echo -e "STREAM FAILED, SW REGS TRACE OK \c"
	    else
		echo -e "STREAM FAILED, SW REGS TRACE FAILED \c"
	    fi
        fi
        
        ${DECODER} -O${TESTDIR}/case_${case_nro}/case_$case_nro.yuv ${TESTDIR}/case_${case_nro}/case_$case_nro.h264 &> ${TESTDIR}/dec_h264.log
	if [ -e ${TESTDIR}/case_${case_nro}/case_${case_nro}.yuv ]
        then
            tmp=`ls -l ${TESTDIR}/case_${case_nro}/case_${case_nro}.yuv | awk '{print $5}'`
	    if [ $tmp -eq 0 ]
	    then
	        echo "case_$case_nro.yuv size 0!"
		exit -1
            fi
	else
	    echo "case_$case_nro.yuv doesn't exist!"
            exit -1    
        fi
	
        ${DECODER} -O${TESTDIR}/case_${case_nro}/ref_case_$case_nro.yuv ${case_dir}/stream.h264 &> dec_h264.log
	if [ -e ${TESTDIR}/case_${case_nro}/ref_case_${case_nro}.yuv ]
        then
            tmp=`ls -l ${TESTDIR}/case_${case_nro}/ref_case_${case_nro}.yuv | awk '{print $5}'`
	    if [ $tmp -eq 0 ]
	    then
	        echo "ref_case_$case_nro.yuv size 0!"
		exit -1
            fi
	else
	    echo "ref_case_$case_nro.yuv doesn't exist!"
            exit -1    
        fi
                
        # Compare output to reference
        findFailingPicture ${TESTDIR}/case_${case_nro}/case_$case_nro.yuv ${TESTDIR}/case_${case_nro}/ref_case_$case_nro.yuv
               
        if [ $? == 0 ]
        then
            echo "Stream differences but YUV data OK"
            #rm -f ${TESTDIR}/case_$case_nro.yuv ${TESTDIR}/ref_case_$case_nro.yuv
            exit 1
        else
            #findFailingPicture case_$case_nro.yuv ref_case_$case_nro.yuv
            echo "FAILED in picture $failing_picture"       
            exit -1
        fi
    fi
)
else
(
    echo "H264 Case $case_nro FAILED (reference 'stream.h264' missing)"
    exit -1
)
fi
 
