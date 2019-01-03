#!/bin/bash


#imports

.  ./f_check.sh

# import commonconfig
if [ ! -e masterscripts/commonconfig.sh ]
then
    echo "<commonconfig.sh> missing"
    exit 1
else
.  ./masterscripts/commonconfig.sh
fi


Usage="\n$0 [-csv]\n\t -csv generate csv file report\n"

csvfile=$csv_path/integrationreport_random_${hwtag}_${swtag}_${reporter}_$reporttime

resultcsv=result.csv

CSV_REPORT=0

# parse argument list
while [ $# -ge 1 ]; do
        case $1 in
        -csv*)  CSV_REPORT=1 ;;
        -*)     echo -e $Usage; exit 1 ;;
        *)      ;;
        esac

        shift
done

if [ $CSV_REPORT -eq 1 ]
then
    echo "TestCase;TestPhase;Category;TestedPictures;Language;StatusDate;ExecutionTime;Status;HWTag;EncSystemTag;SWTag;TestPerson;Comments;Performance;KernelVersion;RootfsVersion;CompilerVersion;TestDeviceIP;TestDeviceVersion" >> $csvfile.csv
fi

cases=`cat random.log | tr "\n" " " | tail -n 1`
random_cases_arr=($cases)

for ((i=0; i<=1999; i++))
do
    case_nro=${random_cases_arr[$i]}
    set_nro=${case_nro}-${case_nro}/5*5

    getExecutionTime "$case_nro"
    
 
    if [ $case_nro -ge 1000 ] && [ $case_nro -lt 2000 ]
    then

    . $test_case_list_dir/test_data_parameter_h264.sh "$case_nro"
			
	if [ ${valid[${set_nro}]} -eq 0 ]
        then
            ttt=`echo | sh checkcase_h264.sh $case_nro`
            res=$?
               
            if [ $CSV_REPORT -eq 1 ]
            then
                echo -n "case_$case_nro;Integration;H264Case;;$language;$timeform1 $timeform2;$execution_time;" >> $csvfile.csv 
                echo -n "case_$case_nro;" >> $resultcsv
                case $res in
                0)
                    echo "OK;$hwtag;$systag;$swtag;$reporter;$comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
	            echo "OK;" >> $resultcsv
		    ;;
                1)
                    extra_comment=${comment}"Stream differences but YUV data OK."
                    echo "FAILED;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
		    echo "FAILED;" >> $resultcsv
		    ;;       
                2)
                    extra_comment=${comment}"$ttt."
                    echo "NOT FAILED;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
		    echo "NOT FAILED;" >> $resultcsv
		    ;; 
                3)
                    extra_comment=${comment}"$ttt."
                    echo "NOT FAILED;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
		    echo "NOT FAILED;" >> $resultcsv
		    ;;
                *)
                    extra_comment=${comment}"$ttt."
                    echo "FAILED;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
		    echo "FAILED;" >> $resultcsv
		    ;; 			    	
                esac
            fi
        
        echo $ttt 
        fi
        
	if [ $case_nro -ge 1750 ] && [ $case_nro -le 1770 ]
        then
	    ttt=`echo | sh checkcase_vs.sh $case_nro`
            res=$?
               
            if [ $CSV_REPORT -eq 1 ]
            then
                echo -n "case_$case_nro;Integration;Standalone stabilization;;$language;$timeform1 $timeform2;$execution_time;" >> $csvfile.csv 
                echo -n "vs_case_$case_nro;" >> $resultcsv
                case $res in
                0)
                    echo "OK;$hwtag;$systag;$swtag;$reporter;$comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv 
		    echo "OK;" >> $resultcsv
		    ;;     
                *)
                    echo "FAILED;$hwtag;$systag;$swtag;$reporter;$comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv 
		    echo "FAILED;" >> $resultcsv
		    ;; 
                esac
             fi
        
        echo $ttt
	fi
    
    elif [ $case_nro -ge 2000 ] && [ $case_nro -lt 3000 ]
    then
    
    . $test_case_list_dir/test_data_parameter_jpeg.sh "$case_nro"
    
    	if [ ${valid[${set_nro}]} -eq 0 ]
        then
            ttt=`echo | sh checkcase_csv.sh $case_nro`
            res=$?
	
            if [ $res -eq 0 ]
            then
	    
                if [ $CSV_REPORT -eq 1 ] 
        	then 
		    
		    echo "case_$case_nro;Integration;JPEGCase;;$language;$timeform1 $timeform2;$execution_time;OK;$hwtag;$systag;$swtag;$reporter;$comment;;$kernelversion;$rootfsversion;$compilerversion;" >> $csvfile.csv ; fi
                    else if [ $res -eq 1 ]
                    then
		 
                        extra_comment=${comment}";Stream differences but YUV data OK"
                    if [ $CSV_REPORT -eq 1 ]; 
		    then 
		    
		        echo "case_$case_nro;Integration;JPEGCase;;$language;$timeform1 $timeform2;$execution_time;FAILED;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;" >> $csvfile.csv ; fi       
                    else if [ $CSV_REPORT -eq 1 ]; 
		    then 
		    	echo "case_$case_nro;Integration;JPEGCase;;$language;$timeform1 $timeform2;$execution_time;FAILED;$hwtag;$systag;$swtag;$reporter;$comment;;$kernelversion;$rootfsversion;$compilerversion;" >> $csvfile.csv ; fi
		    
		 fi
             fi
	    echo "$ttt"
          fi
      fi 
    
    
done
