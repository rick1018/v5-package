#!/bin/bash

#imports
#.   ./f_run.sh

mkdir random_cases

chmod 777 random_cases

randomizeCases()
{
    for ((i=1000; i<=3000; i++))
    do
        let 'j=i-1000' 
        cases[$j]=$i
    done

    let 'number_of_cases=2000'
    let 'i=0'
    
    while [ $number_of_cases != 0 ]
    do

	random_value=$RANDOM; let "random_value %= number_of_cases"

        random_cases_arr[$i]=${cases[$random_value]}

        echo ${cases[$random_value]} >> random.log 

	unset cases[$random_value]
	cases=(${cases[*]})
		
	let 'number_of_cases=number_of_cases-1'
	let 'i=i+1'
    done

    export random_cases_arr
}

randomizeCases

for ((i=0; i<=3000; i++))
do
    
    if [ 1000 -le ${random_cases_arr[$i]} ] && [ ${random_cases_arr[$i]} -lt 2000 ]
    then
        ./test_h264.sh ${random_cases_arr[$i]}
    
        if [ 1750 -le ${random_cases_arr[$i]} ] && [ ${random_cases_arr[$i]} -le 1770 ]
        then
            ./test_vs.sh ${random_cases_arr[$i]}
        fi
    
    elif [ 2000 -le ${random_cases_arr[$i]} ] && [ ${random_cases_arr[$i]} -lt 3000 ]
    then
        ./test_jpeg.sh ${random_cases_arr[$i]}
    fi
done

rm -rf random_run
