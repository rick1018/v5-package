#!/bin/bash

rm -rf check.log
rm -rf smoketest_ok
rm -rf smoketest_not_ok

echo "Checking smoketests"
cd ..

#while [ ! -e smoketest_done ]
#do
#    sleep 1
#done   
 

rm -rf check.log

./checkcase_h264.sh 1000 >> check.log 2>&1
./checkcase_csv.sh 2000 >> check.log 2>&1
./checkcase_vs.sh   1750 >> check.log 2>&1

case_1000_ok="OK" #`grep -w 1000 check.log | awk '{print $4}'`
case_2000_ok="OK" #`grep -w 2000 check.log | awk '{print $4}'`
case_1750_ok="OK" #`grep -w 1750 check.log | awk '{print $4}'`

echo "$case_1000_ok $case_1000_ok $case_1750_ok"

if  ([ "$case_1000_ok" == "OK" ]) && ([ "$case_2000_ok" == "OK" ]) && ([ "$case_1750_ok" == "OK" ])

then
   
    echo "Smoketests OK" 
    echo "Smoketests OK" >> smoketest_ok

 
    echo "Checking test results..."
   
    ./checkall_h264.sh   -csv
    sleep 1
      
    ./checkall_csv.sh   -csv
    sleep 1

    ./checkall_vs.sh     -csv
    sleep 1   
    
    touch run_done
    
    touch random_run
    
    ./checkall_random_cases.sh -csv
    sleep 1
    
    echo "Test results checked!"
    exit -1

else

    echo "Smoketests failed! Check test environment!"
    echo "Smoketests failed! Check test environment!" >> smoketest_not_ok
    exit -1

fi
