#!/bin/sh -f    

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


let "case_nro=$1"

# Test data dir
case_dir=$test_data_home/case_$case_nro

. $test_case_list_dir/test_data_parameter_jpeg.sh "$case_nro"

casetestId=$((testId[set_nro]))

if ( [ "$INTERNAL_TEST" == "n" ] )
then

    if [ $casetestId != "0" ] 
    then
        echo "Internal test, does not work with customer releases!"
        exit 2
    fi
fi

# If test data doesn't exist, generate it
if [ ! -e $case_dir ]
then
    #echo "Directory $case_dir doesn't exist!"
    cd $systag/8270_encoder/system
    ./test_data/test_data.sh $case_nro >> /dev/null 2>&1
    cd ../../../
fi

(
    while [ ! -e ${TESTDIR}/case_${case_nro}/run_${case_nro}_done ]
    do
        sleep 1    
    done

    removeBaseAddress "$TESTDIR/case_$case_nro"
    removeBaseAddress "$case_dir"

    #echo "$TESTDIR/case_$case_nro"


    # Compare stream to reference
    if [ -e ${case_dir}/stream.jpg ]
    then
    (

        echo -e -n "JPEG Case $case_nro\t"

        if (cmp -s ${TESTDIR}/case_${case_nro}/case_$case_nro.jpg ${case_dir}/stream.jpg)
        then
	
            if ( [ "$TRACE_SW_REGISTER" == "y" ] )
	    then
	        if (cmp -s ${TESTDIR}/case_${case_nro}/sw_reg.trc  ${case_dir}/sw_reg.trc)
                then
                    echo -e "STREAM OK, SW REGS TRACE OK \c"
	            exit 0
                else
                    echo -e "STREAM OK, SW REGS TRACE FAILED \c"
	            exit -1
                fi
	    fi 
	    
           echo "STREAM OK"
	   exit 0
        else
	
            if ( [ "$TRACE_SW_REGISTER" == "y" ] )
	    then
	        if (cmp -s ${TESTDIR}/case_${case_nro}/sw_regs_trace ${case_dir}/stream.jpg)
                then
                    echo -e "STREAM FAILED, SW REGS TRACE OK  \c"
	            exit -1
                else
                    echo -e "STREAM FAILED, SW REGS TRACE FAILED \c"
	            exit -1
                fi
	    fi 
	
           echo "STREAM FAILED"
	   exit -1
        fi
	
    )
    else
    (
        echo "FAILED(reference stream.jpg missing)"
	exit -1
    )
    fi
)

 
