#!/bin/sh
# Program:
#	This script support IGD function implement.
# History:
# 2012/06/28	JerryCheng	First release
# 2012/07/27	JerryCheng	add vaild IGD check
#				add external port test
#				del ping IGD check
# 2012/09/27	JerryCheng	modifed for IODATA customer
# 2013/06/24	JerryCheng  	1.update status
#				2.If external IP is different then break
# 2013/09/02	JerryCheng	support ID "miniupnpd" for EDIMAX BR6478n V2 & BR-6408
# 2013/09/10	JerryCheng	marge between IODATA and EDIMAX shell script
#//================================================
# Function
#//================================================
usage(){
	echo "Usage:"
	echo "run_igd.sh ID IP Command"
	echo "or"
	echo "run_igd.sh ID IP Command http_port [rtsp_port]"
	echo "or"
	echo "run_igd.sh ID IP Command http_port external_http_port [rtsp_port] [external_rtsp_port]"
	echo""
	echo""
	echo "Eample Fully-Automation:"
	echo "run_igd.sh camera MyIPCam 192.168.2.3 auto 80 554"
	echo""
	echo "Eample Semi-Automation:"
	echo "run_igd.sh camera MyIPCam 192.168.2.3 semi 80 10000 554 20000"
	rm -rf $UPnP_IGD_PID
	exit 1
}

updateStatus(){
	#param 1: IGD status
	#param 2: external IP addr
	#param 3: external HTTP port
	#param 4: external RTSP port

	if [ ! -z $Internal_RTSP_Port ] || [ ! -z $External_RTSP_Port ]; then
		if [ $IGD_Status == $1 ] && [ $EX_IP == $2 ] && [ $EX_HTTP_Port == $3 ] && [ $EX_RTSP_Port == $4 ]; then
			return;
		fi
	else
		if [ $IGD_Status == $1 ] && [ $EX_IP == $2 ] && [ $EX_HTTP_Port == $3 ]; then
			return;
		fi
	fi
	IGD_Status=$1
	EX_IP=$2
	EX_HTTP_Port=$3
	EX_RTSP_Port=$4

	while [ 1 ]
	do
		sleep 3
		wget "http://127.0.0.1:$Internal_HTTP_Port/camera-cgi/private/updateSysteminfo.cgi?SystemInfo_IGD_Status=$1&SystemInfo_IGD_externalIP=$2&SystemInfo_IGD_exWebPort=$3&SystemInfo_IGD_exAVPort=$4" -O /dev/null -q
		rval=`echo $?`
		if [ "$rval" = 0 ]; then
			break;
		fi
	done

	if [ $Command == "iobb" ]; then
		if [ $1 == "0" ]; then
			logger -p 6 -t "UPnP IGD" "Setting UPnP port failed"
			logger -p 6 -t "UPnP IGD" "Setting UPnP port can not reserve on router"
			echo "Setting UPnP port failed"
			echo "Setting UPnP port can not reserve on router"
		elif [ $1 == "1" ]; then
			logger -p 6 -t "UPnP IGD" "Setting UPnP port failed"
			logger -p 6 -t "UPnP IGD" "UPnP enabled router can not find"
			echo "Setting UPnP port failed"
			echo "UPnP enabled router can not find"
		elif [ $1 == "2" ] || [ $1 == "3" ]; then
			logger -p 6 -t "UPnP IGD" "Setting UPnP port failed"
			logger -p 6 -t "UPnP IGD" "The specified port number have already used"
			echo "Setting UPnP port failed"
			echo "The specified port number have already used"
		elif [ $1 == "4" ]; then
			logger -p 6 -t "UPnP IGD" "find valid UPnP IGD"
			echo "find valid UPnP IGD"
		elif [ $1 == "5" ]; then
			logger -p 6 -t "UPnP IGD" "Setting UPnP port is successful"
			echo "Setting UPnP port is successful"
		elif [ $1 == "6" ]; then
			logger -p 6 -t "UPnP IGD" "Start UPnP IGD"
			echo "Start UPnP IGD"
		elif [ $1 == "7" ]; then
			logger -p 6 -t "UPnP IGD" "Setting UPnP port failed"
			logger -p 6 -t "UPnP IGD" "Setting UPnP port can not add port mapping"
			echo "Setting UPnP port failed"
			echo "Setting UPnP port can not add port mapping"
		fi
	fi
}

checkIDPort(){
	#param 1: ID
	#param 2: compare port
	#return 0=equal 1=not equal
	getIDPort=`cat $UPnP_IGD_INFO | grep "==" | grep $1 | cut -d ' ' -f 4`
	for port in $getIDPort
	do
		#echo "getIDPort is $port, check port is $2"
		if [ $port -eq $2 ]; then
			return 1
		fi
	done
	return 0
}

checkPresentPort(){
	#param 1: ID
	#param 2: compare port
	#return 0=no present 1=present
	getPresetPort=`cat $UPnP_IGD_INFO | grep "==" | cut -d ' ' -f 4`
	for port in $getPresetPort
	do
		#echo "Present port is $port"
		if [ $port -eq $2 ]; then
			is_equal=0
			checkIDPort $1 $port
			is_equal=`echo $?`
			if [ $is_equal == "1" ]; then
				continue;
			else
				return 1
			fi
		fi
	done

	return 0
}

deletePort(){
	#param 1: ID
	#param 2: open port number
	#param 3: command
	#param 4: external HTTP port
	#param 5: external RTSP port
	
	getDeletePort=`cat $UPnP_IGD_INFO | grep "==" | grep $1 | cut -d ' ' -f 4`
	for port in $getDeletePort
	do
		if [ ! -z $port ]; then
			echo "Delete port is $port by ID($1)"
			$UPnP_IGD -d $port tcp > /dev/null 2>&1
		fi
	done

	if [ $3 == "semi" ] || [ $3 == "iobb" ]; then

		checkIDPort "miniupnpd" $4
		is_equal=`echo $?`
		if [ $is_equal == "1" ]; then
			echo "Delete port is $4 by ID(miniupnpd)"
			$UPnP_IGD -d $4 tcp > /dev/null 2>&1
		fi

		if [ $2 == "2" ]; then
			checkIDPort "miniupnpd" $5
			is_equal=`echo $?`
			if [ $is_equal == "1" ]; then
				echo "Delete port is $5 by ID(miniupnpd)"
				$UPnP_IGD -d $5 tcp > /dev/null 2>&1
			fi
		fi
	fi
}

nextTimeCheckIP(){
	#param 1: next minute time
	#return 1=minute is equal 0=minute is not equal
	now_min=`date | cut -d ' ' -f 4 | cut -d ':' -f 2`
	if [ $now_min -ge  $next_min ]; then
		date
		next_min=`expr $now_min + $1`
		return 1
	else
		return 0
	fi
}

restartAgent(){
	if [ $Restart_Agent_Ctrl == "0" ]; then
		Restart_Agent_Ctrl=1
		if [ -f $Agent ]; then
			killall -sigkill agent
			echo "Restart Agent!"
			logger -p 6 -t "UPnP IGD" "Restart Agent!"
		fi
	fi
}

monitorIGD(){
	#param 1: ID
	#param 2: open port number
	#param 3: external HTTP port 
	#param 4: external RTSP port 
	while [ 1 ]
	do
		have_http_port=0
		have_rtsp_port=0
		$UPnP_IGD -l > $UPnP_IGD_INFO

		#1-1.Monitor External Port		
		checkIDPort $1 $3
		is_equal=`echo $?`
		if [ $is_equal == "1" ]; then
			have_http_port=1
		fi
		#echo "check external HTTP Port by ID($1), have_http_port=$have_http_port"

		if [ $have_http_port != "1" ]; then
			checkIDPort "miniupnpd" $3
			is_equal=`echo $?`
			if [ $is_equal == "1" ]; then
				have_http_port=1
			fi
		fi
		#echo "check external HTTP Port by ID(miniupnpd), have_http_port=$have_http_port"

		if [ $2 == "2" ]; then
			checkIDPort $1 $4
			is_equal=`echo $?`
			if [ $is_equal == "1" ]; then
				have_rtsp_port=1
			fi
			#echo "check external HTTP Port by ID($1), have_http_port=$have_http_port"

			if [ $have_rtsp_port != "1" ]; then
				checkIDPort "miniupnpd" $4
				is_equal=`echo $?`
				if [ $is_equal == "1" ]; then
					have_rtsp_port=1
				fi
			fi
			#echo "check external RTSP Port by ID(miniupnpd), have_http_port=$have_http_port, have_rtsp_port=$have_rtsp_port"
		fi

		#1-2.Check External Port Status
		if [ $2 == "1" ]; then
			if [ $have_http_port != "1" ]; then
				echo "External HTTP port disappear"
				break;
			fi
		else
			if [ $have_http_port != "1" ]; then
				echo "External HTTP port disappear"
				break;
			fi
			if [ $have_rtsp_port != "1" ]; then
				echo "External RTSP port disappear"
				break;
			fi
		fi



		#2-1.monitor External IP
		Now_External_IP=`cat $UPnP_IGD_INFO | grep "ExternalIPAddress" | cut -d '=' -f 2`
		if [ "$Now_External_IP" == "0.0.0.0" ] || [ "$Now_External_IP" == "255.255.255.255" ] || [ "$Now_External_IP" == "127.0.0.1" ]; then
			Now_External_IP="0.0.0.0"
		elif [ "$Now_External_IP" == "GetExternalIPAddressFailed" ]; then
			Now_External_IP="0.0.0.0"
		fi

		#2-2.monitor External IP by IOBB CGI
		if [ $Command == "iobb" ]; then
			if [ "$Now_External_IP" == "0.0.0.0" ] || [ "$Now_External_IP" == "255.255.255.255" ] || [ "$Now_External_IP" == "127.0.0.1" ]; then
				#check_ip every 10 minute
				nextTimeCheckIP 10
				rval=`echo $?`
				echo "nextTimeCheckIP rval=$rval"
				if [ $rval == "1" ]; then
					wget http://ipcheck.iobb.net/cgi-bin/ddns/check_ip.cgi -q --timeout 3 --tries 3 -O /tmp/check_ip.html
					rval=`echo $?`
					if [ $rval == "0" ]; then
						if [ -f /tmp/check_ip.html ]; then
							Now_External_IP=`grep "Current IP Address:" /tmp/check_ip.html  | cut -d ":" -f 2`
							rm -rf /tmp/check_ip.html
						fi
					fi
				fi
			fi
		fi

		#2-3.Check External IP Status
		if [ -z $Now_External_IP ]; then
			Now_External_IP="0.0.0.0"
		fi
		if [ -z $External_IP ]; then
			External_IP=$Now_External_IP
		fi

		if [ "$Now_External_IP" != "0.0.0.0" ] && [ "$Now_External_IP" != "$External_IP" ]; then
			logger -p 6 -t "UPnP IGD" "old External IP: $External_IP, new External IP $Now_External_IP"
			External_IP=$Now_External_IP
			if [ $Command == "semi" ] || [ $Command == "iobb" ]; then
				updateStatus 5 $External_IP $External_HTTP_Port $External_RTSP_Port
			else
				updateStatus 5 $External_IP $Random_HTTP_Port $Random_RTSP_Port
			fi
			break;
		fi

		#kill agent for PNV
		restartAgent

		sleep 120
	done
}

Fully_Automation(){
	#param 1: ID
	#param 2: IP
	#param 3: Port
	#param 4: Message
	$UPnP_IGD -l > $UPnP_IGD_INFO
	is_equal=0
	random_port=""

	#getRandomPort
	while [ 1 ]
	do
		echo $RANDOM > /tmp/random
		random_port=`cat /tmp/random`
		if [ $random_port -ge "10000" -a $random_port -le "30000" ]; then
			is_equal=0
			checkPresentPort $1 $random_port
			is_equal=`echo $?`
			if [ $is_equal == "0" ]; then
				break;
			fi
		fi
	done
	rm -rf /tmp/random

	# add port
	echo "Add port is $3 by ID($1)"
	$UPnP_IGD -a $2 $3 $random_port tcp $1 > /dev/null 2>&1
	echo "Open external $4 port at $random_port"
	if [ $4 == "HTTP" ]; then
		Random_HTTP_Port=$random_port
	else
		Random_RTSP_Port=$random_port
	fi
}

Semi_Automation(){
	#param 1: ID
	#param 2: IP
	#param 3: Internal Port
	#param 4: External Port
	#param 5: Message
	#return 0:open port ok, 1:port opened by someone, 2: open port fail.
	$UPnP_IGD -l > $UPnP_IGD_INFO
	EQUAL_PORT=0
	checkPresentPort $1 $4
	EQUAL_PORT=`echo $?`
	
	#echo "check PresentPort by ID($1), EQUAL_PORT=$EQUAL_PORT"
	if [ $EQUAL_PORT == "1" ]; then
		checkPresentPort "miniupnpd" $4
		EQUAL_PORT=`echo $?`
		#echo "check PresentPort by miniupnpd, EQUAL_PORT=$EQUAL_PORT"
		if [ $EQUAL_PORT == "1" ]; then
			if [ $5 == "HTTP" ]; then
				echo "HTTP port have opened!"
			else
				echo "RTSP port have opened!"
			fi
			return 1
		fi
	else
		echo "Add port is $4 by ID($1)"
		$UPnP_IGD -a $2 $3 $4 tcp $1 > /dev/null 2>&1
		if [ $5 == "HTTP" ]; then
			echo "Open external HTTP port at $4"
		else
			echo "Open external RTSP port at $4"
		fi

		if [ $Command == "iobb" ]; then
			#Is open port successfully ?
			$UPnP_IGD -l > $UPnP_IGD_INFO
			checkIDPort $1 $4
			is_equal=`echo $?`
			#echo "check checkIDPort by ID($1), is_equal=$is_equal"
			if [ $is_equal == "1" ]; then
				return 0
			else
				checkIDPort "miniupnpd" $4
				is_equal=`echo $?`
				#echo "check checkIDPort by ID(miniupnpd), is_equal=$is_equal"
				if [ $is_equal == "1" ]; then
					return 0
				else
					return 2
				fi
			fi
		else
			return 0;
		fi
	fi
}

testIGD()
{
	IGDStatus=`cat $UPnP_IGD_INFO | grep "Found valid IGD"`
	rval=`echo $?`
	if [ $rval == "1" ]; then
		IGDStatus=`cat $UPnP_IGD_INFO | grep "Found a (not connected?) IGD"`
		rval=`echo $?`
		if [ $rval == "1" ]; then
			#echo "No find IGD"
			updateStatus 1 "" "" ""
			return 1
		fi
	fi

	if [ $Command == "semi" ] || [ $Command == "iobb" ]; then
		#Open external HTTP port
		equal_port=0
		checkPresentPort $ID $External_HTTP_Port
		equal_port=`echo $?`
		#echo "check HTTP PresentPort by ID($ID), equal_port=$equal_port"
		if [ $equal_port == "1" ]; then
			checkPresentPort "miniupnpd" $External_HTTP_Port
			equal_port=`echo $?`
			#echo "check PresentPort by miniupnpd, equal_port=$equal_port"
			if [ $equal_port == "1" ]; then
				echo "HTTP port have opened!"
				updateStatus 2 "" "" ""
				return 1
			fi
	        fi

        	#Open external RTSP port
		if [ ! -z $External_RTSP_Port ]; then
			equal_port=0
			checkPresentPort $ID $External_RTSP_Port
	                equal_port=`echo $?`
			#echo "check RTSP PresentPort by ID($ID), equal_port=$equal_port"
			if [ $equal_port == "1" ]; then
				checkPresentPort "miniupnpd" $External_RTSP_Port
	                	equal_port=`echo $?`
				#echo "check RTSP PresentPort by miniupnpd, equal_port=$equal_port"
				if [ $equal_port == "1" ]; then
					echo "RTSP port have opened!"
					updateStatus 3 "" "" ""
					return 1
				fi
			fi
		fi
	fi

	updateStatus 4 "" "" ""
	return 0
}

findIGD(){
	while [ 1 ]
	do
		$UPnP_IGD -l > $UPnP_IGD_INFO

		IP=`ifconfig | grep "inet addr:" | head -n 1 | cut -d ':' -f 2 | cut -d ' ' -f 0`
		if [ $Command == "semi" ] || [ $Command == "iobb" ]; then
			testIGD $1 $IP $3 $4 $5 $6 $7 
		else
			testIGD $1 $IP $3 $4 $5
		fi

		rval=`echo $?`
		if [ $rval == "0" ]; then
			External_IP=`cat $UPnP_IGD_INFO | grep "ExternalIPAddress" | cut -d '=' -f 2`
			if [ "$External_IP" == "0.0.0.0" ] || [ "$External_IP" == "127.0.0.1" ]; then
				External_IP="0.0.0.0"
			elif [ "$External_IP" == "GetExternalIPAddressFailed" ]; then
				External_IP="0.0.0.0"
			fi

			if [ $Command == "iobb" ]; then
				if [ "$External_IP" == "0.0.0.0" ]; then
					#check_ip every 10 minute
					nextTimeCheckIP 10
					rval=`echo $?`
					if [ $rval == "1" ]; then
						wget http://ipcheck.iobb.net/cgi-bin/ddns/check_ip.cgi -q --timeout 3 --tries 3 -O /tmp/check_ip.html
						rval=`echo $?`
						if [ $rval == "0" ]; then
							if [ -f /tmp/check_ip.html ]; then
								External_IP=`grep "Current IP Address:" /tmp/check_ip.html  | cut -d ":" -f 2`
								rm -rf /tmp/check_ip.html
							fi
						fi
					fi
				fi
			fi
			break;
		fi
		sleep 120
	done
}
#//================================================
#main function

UPnP_IGD=/bin/upnpc-static
UPnP_IGD_PID=/tmp/run_igd.pid
UPnP_IGD_INFO=/tmp/igd_info.txt
UPnP_IGD_CMD=/tmp/igd_cmd.txt
Agent=/bin/agent
Restart_Agent_Ctrl=0

# get pid of process
echo "$$" > $UPnP_IGD_PID

#check input parameter
if [ $# == 0 ]; then
	usage
fi

#read setting
ID=$1
IP=$2
Command=$3
echo "ID is $1"
echo "IP is $2"
if [ $Command == "semi" ] || [ $Command == "iobb" ]; then
	echo "Command is Semi-Automation($Command)"
	Internal_HTTP_Port=$4
	External_HTTP_Port=$5
	Internal_RTSP_Port=$6
	External_RTSP_Port=$7
	echo "Internal HTTP port is $4"
	echo "External HTTP port is $5"
	if [ ! -z $Internal_RTSP_Port ]; then
		echo "Internal RTSP port is $6"
		echo "External RTSP port is $7"
	fi
elif [ $Command == "auto" ]; then
	echo "Command is Fully-Automation"
	Internal_HTTP_Port=$4
	Internal_RTSP_Port=$5
	Random_HTTP_Port=0
	Random_RTSP_Port=0
	echo "Internal HTTP port is $4"
	echo "External HTTP port is random"
	if [ ! -z $Internal_RTSP_Port ]; then
		echo "Internal RTSP port is $5"
		echo "External RTSP port is random"
	fi
fi

#for IODATA
now_min=`date | cut -d ' ' -f 4 | cut -d ':' -f 2`
next_min=$now_min

#clear opened port
IGD_Status=255
updateStatus 6 "" "" ""

External_IP="0.0.0.0"
Now_External_IP="0.0.0.0"

#Open port
if [ $Command == "semi" ] || [ $Command == "iobb" ]; then
	while [ 1 ]
	do
		#1.Find IGD & get WAN IP
		findIGD $1 $2 $3 $4 $5 $6 $7
		if [ "$External_IP" == "0.0.0.0" ]; then
			sleep 120
			continue;
		fi

		#2.Serched port by camera name & delete old port
		if [ ! -z $Internal_RTSP_Port ] || [ ! -z $External_RTSP_Port ]; then
			deletePort $ID 2 $Command $External_HTTP_Port $External_RTSP_Port
		else
			deletePort $ID 1 $Command $External_HTTP_Port
		fi

		#3.Open external HTTP & RTSP port
		Semi_Automation $ID $IP $Internal_HTTP_Port $External_HTTP_Port HTTP
		rval=`echo $?`
		if [ $rval == "1" ]; then
			updateStatus 2 "" "" ""
			sleep 10
			continue;
		elif [ $rval == "2" ]; then
			updateStatus 7 "" "" ""
			sleep 10
			continue;
		fi

		if [ ! -z $Internal_RTSP_Port ] || [ ! -z $External_RTSP_Port ]; then
			Semi_Automation $ID $IP $Internal_RTSP_Port $External_RTSP_Port RTSP
			rval=`echo $?`
			if [ $rval == "1" ]; then
				updateStatus 3 "" "" ""
				echo "Delete just open port $External_HTTP_Port!"
				$UPnP_IGD -d $External_HTTP_Port tcp > /dev/null 2>&1
				sleep 10
				continue;
			elif [ $rval == "2" ]; then
				updateStatus 7 "" "" ""
				echo "Delete just open port $External_HTTP_Port!"
				$UPnP_IGD -d $External_HTTP_Port tcp > /dev/null 2>&1
				sleep 10
				continue;
			fi
		fi

		#4.Update IGD status
		updateStatus 5 $External_IP $External_HTTP_Port $External_RTSP_Port

		#5.check external IP and Port
		if [ ! -z $Internal_RTSP_Port ] || [ ! -z $External_RTSP_Port ]; then
		monitorIGD $ID 2 $External_HTTP_Port $External_RTSP_Port
		else
		monitorIGD $ID 1 $External_HTTP_Port
		fi
		updateStatus 0 "" "" ""

		#clean flag
		Restart_Agent_Ctrl=0

		sleep 60
	done
elif [ $Command == "auto" ]; then
        while [ 1 ]
        do
		#1.Find IGD & get WAN IP
		findIGD $1 $2 $3 $4 $5 $6 $7
		if [ "$External_IP" == "0.0.0.0" ]; then
			sleep 120
			continue;
		fi

		#2.Serched port by camera name & delete old port
		if [ ! -z $Internal_RTSP_Port ] || [ ! -z $External_RTSP_Port ]; then
			deletePort $ID 2 $Random_HTTP_Port $Random_RTSP_Port
		else
			deletePort $ID 1 $Random_HTTP_Port
		fi

		#3.Open external HTTP & RTSP port
		Fully_Automation $ID $IP $Internal_HTTP_Port HTTP
		if [ ! -z $Internal_RTSP_Port ]; then
			Fully_Automation $ID $IP $Internal_RTSP_Port RTSP
		fi

		#4.Update IGD status
		updateStatus 5 $External_IP $Random_HTTP_Port $Random_RTSP_Port

		#5.check external IP and Port
		if [ ! -z $Internal_RTSP_Port ] || [ ! -z $External_RTSP_Port ]; then
			monitorIGD $ID 2 $Random_HTTP_Port $Random_RTSP_Port
		else
			monitorIGD $ID 1 $Random_HTTP_Port
		fi
		updateStatus 0 "" "" ""

		#clean flag
		Restart_Agent_Ctrl=0

		sleep 60
	done
fi

rm -rf $UPnP_IGD_INFO $UPnP_IGD_PID $UPnP_IGD_CMD
exit 0
