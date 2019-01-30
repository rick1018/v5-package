#!/bin/sh

# get pid of process
echo "$$" > /tmp/run_dhcpc.pid

#kill previous DHCP client
if [ -f /tmp/udhcpc-br0.pid ]; then	
	PID=`cat /tmp/udhcpc-br0.pid`
	if [ ! -z "$PID" ]; then
		kill - 9 $PID
		rm -rf udhcpc-br0.pid
	fi
fi

STATIC_IP=""
NETMASK=""
GETEWAY=""

if [ -f /tmp/ipcamConfig.xml ]; then
	STATIC_IP=`/bin/getIPCamConfig SystemConfig_Network_IP`
	NETMASK=`/bin/getIPCamConfig SystemConfig_Network_netmask`
	GETEWAY=`/bin/getIPCamConfig SystemConfig_Network_gateway`
        ifconfig br0 $STATIC_IP netmask $NETMASK
        route add default gw $GETEWAY
        echo "Default IP: $STATIC_IP"
else
        STATIC_IP="192.168.2.3"
        ifconfig br0 192.168.2.3 netmask 255.255.255.0
        route del default gw
	route add -host 255.255.255.255 dev br0
fi

while [ -f /bin/udhcpc ]
do
	udhcpc -i br0 -p /tmp/udhcpc-br0.pid -s /etc/script/br0.sh renew
	RESULT=`echo $?`
        sleep 3
        if [ "$RESULT" = 0 ]; then
                route -n
                rm -rf /tmp/run_dhcpc.pid
                exit 0
        fi
done
