#!/bin/sh
# Program:
#	This script support IGD function implement.
# History:
# 2012/06/28	JerryCheng	First release

cp -f /bin/busybox /tmp/reboot

while [ 1 ]
do
	sleep 1
	if [ -f /tmp/upgradeFW_OK ]; then
		/tmp/reboot -d 5
		break;
	fi
done
