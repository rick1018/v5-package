#rmmod rt2860v2_ap.ko
#rmmod rt2860v2_sta.ko
killall -9 avi_rec
killall -9 avi_event
echo "Start killall.sh"
killall -9 wis-streamer
killall -15 finder_server
killall -9 upnp-scanip
killall -9 dhcpcd
sleep 2
killall -9 boa
#killall -9 nginx
killall -9 av_server 
killall -9 system_server
echo "End killall.sh!"

