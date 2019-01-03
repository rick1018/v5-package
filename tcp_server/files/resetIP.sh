#!/bin/sh -x 

COUNT=0
SERIAL="/dev/ttyS2"
while [ 1 ]
do
    [ -z "$(ifconfig eth0 |grep 'inet addr')" ] && {
        ifconfig eth0 $1 netmask 255.255.255.0
    }
    
    [ "$1" == "192.168.0.101" ] && {
	ping -c 1 -w 1 192.168.0.102 && result=0 || result=1 
	[ "$result" == "0" ] && {
	     COUNT=$(($COUNT + 1))
             [ $COUNT -gt 5 -a -z "$(ps |grep tcp_server|grep -v grep)" ] && {
		#/usr/bin/tcp_server > /tmp/server.log 2>&1 &
		/usr/bin/tcp_server > /dev/null 2>&1 &
		COUNT=0
             }
	     [ -z "$(ps |grep remote_control|grep -v grep)" ] && {
		/usr/bin/remote_control $SERIAL 0 > /dev/null 2>&1 &
	     }
	}
	[ "$result" == "1" ] && {
	    COUNT=0
	}
    }

    [ "$1" == "192.168.0.102" ] && {
	ping -c 1 -w 1 192.168.0.101 && result=0 || result=1
	[ "$result" == "0" ] && {
	     COUNT=$(($COUNT + 1))
	     [ $COUNT -gt 5 -a -z "$(ps |grep tcp_client|grep -v grep)" ] && {
	        #/usr/bin/tcp_client > /tmp/client.log 2>&1 &
	        /usr/bin/tcp_client > /dev/null 2>&1 &
		COUNT=4
	     }
	     [ -z "$(ps |grep remote_control|grep -v grep)" ] && {
		/usr/bin/remote_control $SERIAL 1 > /dev/null 2>&1 &
	     }
	}
	[ "$result" == "1" ] && {
	    COUNT=0
	}
    }

    sleep 1
done
