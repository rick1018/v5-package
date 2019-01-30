#!/bin/sh

#[1] get pid of process
echo "$$" > /tmp/run_pppoe.pid

#[2] kill previous pppd
pidfile="/var/run/ppp0.pid"
if [ -f "$pidfile" ]; then
	PID=`cat "$pidfile"`
	if [ -n "$PID" ]; then
		kill - 9 $PID
		rm -rf "$pidfile"
	fi
fi

#[3] get PPPoE seting
if [ -z "$1" ]; then
	echo "Error: no param, \"run_pppoe.sh mtu username passwd\""
	exit 1
else
	MTU="$1"
	MRU="$1"
fi
if [ -n "S2" ]; then
	USERNAME="$2"
else
	USERNAME=""
fi
if [ -n "$3" ]; then
	PASSWD="$3"
else
	PASSWD=""
fi

optionsfile="/etc/ppp/options"
papfile="/etc/ppp/pap-secrets"
chapfile="/etc/ppp/chap-secrets"

#[4] set pppoe options
echo "noipdefault" > $optionsfile
echo "usepeerdns" >> $optionsfile
echo "noauth" >> $optionsfile
echo "default-asyncmap" >> $optionsfile
echo "defaultroute" >> $optionsfile
echo "hide-password" >> $optionsfile
echo "nodetach " >> $optionsfile
echo "mtu $MTU" >> $optionsfile
echo "mru $MRU" >> $optionsfile
echo "noaccomp" >> $optionsfile
echo "noccp" >> $optionsfile
echo "nobsdcomp" >> $optionsfile
echo "nodeflate" >> $optionsfile
echo "nopcomp" >> $optionsfile
echo "novj" >> $optionsfile
echo "novjccomp" >> $optionsfile
echo "user $USERNAME" >> $optionsfile
echo "lcp-echo-interval 30" >> $optionsfile
echo "lcp-echo-failure 2" >> $optionsfile
echo "plugin /etc/ppp/plugins/libplugin.a" >> $optionsfile

#[5] set pppoe auth
if [ -n "$PASSWD" ]; then
	echo "$USERNAME	*	$PASSWD	*" > $papfile
	echo "$USERNAME	*	$PASSWD	*" > $chapfile
else
	echo "no auth"
fi

#[6] connect loop
while [ True ]; do
	if [ ! "`ifconfig | grep "^ppp0[[:blank:]]"`" == "" ] && [ -r "/var/run/ppp0.pid" ]; then
		sleep 60
	else
		if [ -r "/tmp/lan_resolv.conf" ]; then
			cat /tmp/lan_resolv.conf > /etc/resolv.conf
		fi
		pid=`ps | grep pppd | cut -c 1-5 | head -n 1`
		if [ "$pid" != "" ]; then
			kill -9 $pid 2> /dev/null
		fi	
		rm /var/run/ppp0.pid 2> /dev/null
		sleep 1

		pppd br0 &
		sleep 5
		if [ ! "`ifconfig | grep "^ppp0[[:blank:]]"`" == "" ] && [ -r "/var/run/ppp0.pid" ]; then
			cat /etc/resolv.conf > /tmp/lan_resolv.conf
			cat /etc/ppp/resolv.conf > /etc/resolv.conf
			
			#restart agent
			killall -sigkill agent
		fi
	fi
done

