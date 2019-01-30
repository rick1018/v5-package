#!/bin/sh

# udhcpc script edited by Tim Riker <Tim@Rikers.org>

[ -z "$1" ] && echo "Error: should be called from udhcpc" && exit 1

RESOLV_CONF="/etc/resolv.conf"

[ -n "$broadcast" ] && BROADCAST="broadcast $broadcast"
[ -n "$subnet" ] && NETMASK="netmask $subnet"

set_default_IP() {
        if [ -f /tmp/ipcamConfig.xml ]; then
		STATIC_IP=`/bin/getIPCamConfig SystemConfig_Network_IP`
		NETMASK=`/bin/getIPCamConfig SystemConfig_Network_netmask`
		GETEWAY=`/bin/getIPCamConfig SystemConfig_Network_gateway`
                ifconfig br0 $STATIC_IP netmask $NETMASK
                route add default gw $GETEWAY
		route add -host 255.255.255.255 dev $interface
                echo "Default IP: $STATIC_IP"
        else
                STATIC_IP="192.168.2.3"
                ifconfig br0 192.168.2.3 netmask 255.255.255.0
                route del default gw
        fi
}

count=0
IP="0.0.0.0"
NET_MASK="0.0.0.0"
NET_GATEWAY="0.0.0.0"
DNS1="0.0.0.0"
DNS2="0.0.0.0"
ntpServer="0.0.0.0"
NETWORK_TYPE=`/bin/getIPCamConfig SystemConfig_Network_networkType`
BEFORE_IP=`/bin/getIPCamConfig SystemInfo_DHCP_IP`
case "$1" in
	deconfig)
		#DHCP get IP failed. Setting default network value.
                set_default_IP
		;;

	renew|bound)
		ifconfig $interface $ip $BROADCAST $NETMASK
		if [ -n "$router" ] ; then
			echo "deleting routers"
			while route del default gw 0.0.0.0 dev $interface ; do
				:
			done

			for i in $router ; do
				route add default gw $i dev $interface
				if [ "$count" == 0 ]; then
                                	NET_GATEWAY=$i
	                        fi
				count=`expr $count + 1`
			done
		fi

		route add -host 255.255.255.255 dev $interface

		echo -n > $RESOLV_CONF
		[ -n "$domain" ] && echo search $domain >> $RESOLV_CONF
		count=0;
		for i in $dns ; do
			echo adding dns $i
			echo nameserver $i >> $RESOLV_CONF

			if [ "$count" == 0 ]; then
				DNS1=$i
        		elif [ "$count" == 1 ]; then
				DNS2=$i
        		elif [ "$count" == 2 ]; then
				DNS3=$i
        		fi
			count=`expr $count + 1`

		done

		echo "DNS1:$DNS1"
		echo "DNS2:$DNS2"
		echo "DNS3:$DNS3"
		echo "count:$count"
		count=0;

		for i in $ntpsrv ; do
			if [ "$count" == 0 ]; then
				ntpServer=$i
        		fi
			count=`expr $count + 1`
		done
		
		count=0;
		for i in $ip ; do
			if [ "$count" == 0 ]; then
				IP=$i
        		fi
			count=`expr $count + 1`
		done

		count=0;
		for i in $subnet ; do
			if [ "$count" == 0 ]; then
				NET_MASK=$i
        		fi
			count=$count+1
		done
		
		#add default DDNS server
		echo nameserver "8.8.8.8" >> $RESOLV_CONF

		#restart Bonjour
		if [ -f /bin/mDNSResponderPosix ]; then
			if [ -f /var/run/mDNSResponder.pid ]; then
				kill -15 `cat /var/run/mDNSResponder.pid`
				#sleep 1
			fi 
			/bin/mDNSResponderPosix -b -f /tmp/Services.txt
		fi

		#restart agent
		if [ -f /bin/agent ]; then
			killall -sigkill agent
		fi

		WEB_PORT=`grep "<httpPort maxLen=\"5\">" /tmp/ipcamConfig.xml | cut -d '>' -f 2 | cut -d '<' -f 1`
		while [ 1 ]
		do
        		sleep 6
                	wget "http://127.0.0.1:$WEB_PORT/camera-cgi/private/updateSysteminfo.cgi?SystemInfo_DHCP_IP=$IP&SystemInfo_DHCP_netmask=$NET_MASK&SystemInfo_DHCP_gateway=$NET_GATEWAY&SystemInfo_DHCP_dns1=$DNS1&SystemInfo_DHCP_dns2=$DNS2&SystemInfo_DHCP_dns3=$DNS3&SystemInfo_DHCP_ntpServer=$ntpServer" -O /dev/null -q
			RESULT=`echo $?`
	        	if [ "$RESULT" = 0 ]; then
				break;
        		fi
		done

		#restart IGD
		echo "NETWORK_TYPE=$NETWORK_TYPE"
		echo "BEFORE_IP=$BEFORE_IP"
		echo "IP=$IP"
		if [ "$NETWORK_TYPE" == "1" -a "$BEFORE_IP" == "$IP" ]; then
			#If renew the same as ip. Don't run idg.
			exit 0
		fi

		IGD_PID=`cat /tmp/run_igd.pid`
		if [ ! -z $IGD_PID ]; then
			kill -9 $IGD_PID
			rm -rf /tmp/run_igd.pid
			ID=`cat /tmp/igd_cmd.txt | grep id | cut -d ':' -f 2`	
			TYPE=`cat /tmp/igd_cmd.txt | grep type | cut -d ':' -f 2`
			InWebPort=`cat /tmp/igd_cmd.txt | grep inWebPort | cut -d ':' -f 2`
			ExWebPort=`cat /tmp/igd_cmd.txt | grep exWebPort | cut -d ':' -f 2`
			InRTSPPort=`cat /tmp/igd_cmd.txt | grep inAVPort | cut -d ':' -f 2`
			ExRTSPPort=`cat /tmp/igd_cmd.txt | grep exAVPort | cut -d ':' -f 2`
			if [ $TYPE == "iobb" ]; then
				if [ -z $InRTSPPort ]; then
					/etc/script/run_igd.sh $ID $IP iobb $InWebPort $ExWebPort &
				else
					/etc/script/run_igd.sh $ID $IP iobb $InWebPort $ExWebPort $InRTSPPort $ExRTSPPort &
				fi
			elif [ $TYPE == "auto" ]; then
				if [ -z $InRTSPPort ]; then
					/etc/script/run_igd.sh $ID $IP auto $InWebPort &
				else
					/etc/script/run_igd.sh $ID $IP auto $InWebPort $InRTSPPort &
				fi
			else
				if [ -z $InRTSPPort ]; then
					/etc/script/run_igd.sh $ID $IP semi $InWebPort $ExWebPort &
				else
					/etc/script/run_igd.sh $ID $IP semi $InWebPort $ExWebPort $InRTSPPort $ExRTSPPort &
				fi
			fi
		fi
		;;
esac

exit 0
