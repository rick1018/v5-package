#!/bin/bash


cd ..
/sbin/rmmod memalloc
./memalloc_load.sh alloc_method=2
/sbin/rmmod hx280enc
./driver_load.sh


#Execute smoketests

rm -rf smoketest_ok
rm -rf smoketest_not_ok
rm -rf run_done
rm -rf smoketest_done

echo "Executing smoketests"

./test_h264.sh 1000 
./test_jpeg.sh 2000
./test_vs.sh 1750

touch smoketest_done

while [ 1 ]
do

sleep 1

    if [ -e smoketest_ok ]
    then
    
        echo "Waiting for test cases to finish..."
    
        ./test_h264.sh all
        ./test_jpeg.sh all
        ./test_vs.sh all
	
	while [ ! -e random_run ]
	do
	    sleep 1
	done
	
	./test_random_cases.sh
	
        echo "Test cases finished!"
    
        touch run_done
    
        exit -1
    
    elif [ -e smoketest_not_ok ]    
    then
    exit -1
     
    fi  
       
done     
