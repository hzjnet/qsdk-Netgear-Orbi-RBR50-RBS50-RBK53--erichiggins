#!/bin/sh

mtd_oops="$(part_dev crashinfo)"

echo "Save the collect log into debug-log.zip and upload to user"

#Disblae wireless debug log
iwpriv ath0 dbgLVL 0x0
iwpriv ath1 dbgLVL 0x0
iwpriv ath01 dbgLVL 0x0
iwpriv ath2 dbgLVL 0x0

module_name=`cat /module_name`

# Save the router config file
/bin/config backup /tmp/NETGEAR_$module_name.cfg

#/sbin/debug_save_panic_log $mtd_oops

cd /tmp

# System will zipped all debug files into 1 zip file and save to client browser
# So a debug-log.zip file will includes
# (1) Console log
# (2) Basic debug information
# (3) router config file
# (4) LAN/WAN packet capture
# (5) thermal log
# (6) debug here log of hyt 

#Disable the capture
killall tcpdump
killall tcpdump
killall basic_log.sh 
killall console_log.sh 
killall wireless_log.sh  
killall thermal_log.sh  
killall debug_here_log.sh
/bin/config set netscan_debug=0

#AS long as user click "Save logs" form debug page, the current HYCTL log should be generated and captured
if [ -f /tmp/wireless-log1.txt ]; then
	echo "AS long as user click "Save logs" form debug page, the current HYCTL log should be generated and captured" >>/tmp/wireless-log1.txt
	echo -n "gethatbl:" >>/tmp/wireless-log1.txt;	hyctl gethatbl br0 1000 >>/tmp/wireless-log1.txt	
	echo -n "gethdtbl:" >>/tmp/wireless-log1.txt;   hyctl gethdtbl br0 100 >>/tmp/wireless-log1.txt	
	echo -n "getfdb:" >>/tmp/wireless-log1.txt;   hyctl getfdb br0 100 >>/tmp/wireless-log1.txt
elif [ -f /tmp/wireless-log2.txt ]; then
	echo "AS long as user click "Save logs" form debug page, the current HYCTL log should be generated and captured" >> /tmp/wireless-log2.txt
	echo -n "gethatbl:" >>/tmp/wireless-log2.txt;	hyctl gethatbl br0 1000 >>/tmp/wireless-log2.txt	
	echo -n "gethdtbl:" >>/tmp/wireless-log2.txt;   hyctl gethdtbl br0 100 >>/tmp/wireless-log2.txt	
	echo -n "getfdb:" >>/tmp/wireless-log2.txt;   hyctl getfdb br0 100 >>/tmp/wireless-log2.txt
fi
	

echo close > /sys/devices/platform/serial8250/console

collect_log=`cat /tmp/collect_debug`


if [ "x$collect_log" = "x0" ];then
	/sbin/basic_log.sh &
	sleep 20
	killall basic_log.sh 
fi


dd if=$mtd_oops of=/tmp/panic_log.txt bs=131072 count=2
[ -f /tmp/panic_log.txt ] && unix2dos /tmp/panic_log.txt
#[ -f /tmp/Panic-log.txt ] && unix2dos /tmp/Panic-log.txt
[ -f /tmp/Console-log1.txt ] && unix2dos /tmp/Console-log1.txt
[ -f /tmp/Console-log2.txt ] && unix2dos /tmp/Console-log2.txt 
[ -f /tmp/logread-log1.txt ] && unix2dos /tmp/logread-log1.txt
[ -f /tmp/logread-log2.txt ] && unix2dos /tmp/logread-log2.txt
[ -f /tmp/wireless-log1.txt ] && unix2dos /tmp/wireless-log1.txt 
[ -f /tmp/wireless-log2.txt ] && unix2dos /tmp/wireless-log2.txt 
[ -f /tmp/basic_debug_log.txt ] && unix2dos /tmp/basic_debug_log.txt
[ -d /tmp/soapclient ] && unix2dos /tmp/soapclient/*
[ -d /var/log/soapclient ] && unix2dos /var/log/soapclient/*
[ -f /var/log/soapapp ] && unix2dos /var/log/soapapp
[ -f /tmp/hyt_result ] && unix2dos /tmp/hyt_result
[ -f /tmp/satellite_status ] && unix2dos /tmp/satellite_status
[ -e /tmp/radardetect.log ] && RADARLOG=radardetect.log
[ -f /tmp/thermal-log1.txt ] && unix2dos /tmp/thermal-log1.txt
[ -f /tmp/thermal-log2.txt ] && unix2dos /tmp/thermal-log2.txt

[ -f /tmp/debug_here_log_1.txt ] && unix2dos /tmp/debug_here_log_1.txt
[ -f /tmp/debug_here_log_2.txt ] && unix2dos /tmp/debug_here_log_2.txt


if [ "x$collect_log" = "x1" ];then
	zip debug-log.zip  NETGEAR_$module_name.cfg panic_log.txt /firmware_version Console-log1.txt Console-log2.txt logread-log1.txt logread-log2.txt thermal-log1.txt thermal-log2.txt basic_debug_log.txt wireless-log1.txt wireless-log2.txt lan.pcap wan.pcap soapclient/* /var/log/soapclient/* /var/log/soapapp hyt_result satellite_status hyd-restart.log wsplcd-restart.log debug_here_log_1.txt debug_here_log_2.txt dhcpd_hostlist /var/log/netscan/* device_tables/local_device_table $RADARLOG
else
	zip debug-log.zip NETGEAR_$module_name.cfg  panic_log.txt /firmware_version Console-log1.txt Console-log2.txt logread-log1.txt logread-log2.txt thermal-log1.txt thermal-log2.txt wireless-log1.txt wireless-log2.txt basic_debug_log.txt lan.pcap wan.pcap soapclient /var/log/soapclient/* /var/log/soapapp hyt_result satellite_status hyd-restart.log wsplcd-restart.log debug_here_log_1.txt debug_here_log_2.txt dhcpd_hostlist /var/log/netscan/* device_tables/local_device_table $RADARLOG
fi

cd /tmp
rm -rf debug-usb debug_cpu debug_flash debug_mem debug_mirror_on debug_session NETGEAR_$module_name.cfg panic_log.txt Console-log1.txt Console-log2.txt logread-log1.txt logread-log2.txt thermal-log1.txt thermal-log2.txt basic_debug_log.txt lan.pcap wan.pcap wireless-log1.txt wireless-log2.txt /var/log/soapapp debug_here_log_1.txt debug_here_log_2.txt

echo 0 > /tmp/collect_debug
