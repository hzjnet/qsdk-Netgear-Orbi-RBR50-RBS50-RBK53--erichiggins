#! /bin/sh
#Check config var streamboost_enable every hour
update_count=0
while :; do
	sleep 60
	update_count=$(($update_count + 1))
	if [ $update_count = 3 ]; then
		update_count=0
		if [ "x`/bin/config get enable_circle_plc`" = "x1" -a "x`/bin/config get ap_mode`" = "x0" ]; then
			/bin/config set "save_circle_info=1"
			killall -SIGUSR1 net-scan
		fi
	fi
	streamboost_enable=`config get streamboost_enable`
	if [ "x$streamboost_enable" = "x1" ];then
		streamboost status > /tmp/streamboost_status
		downnum=`cat /tmp/streamboost_status |grep DOWN |wc -l`
		if [ "$downnum" != "0" ] && [ "$downnum" -lt "10" ]; then
			/etc/init.d/streamboost restart
			time=`date '+%Y-%m-%dT%H:%M:%SZ'`
			echo "Restart streamboost:$time" >> /tmp/restart_process_list
		fi
	fi
	status=`ps | grep soap_agent | grep -v grep`
	if [ -z "$status" ] && [ "$(cat /tmp/orbi_type)" = "Base" ];then
		killall soap_agent
		time=`date '+%Y-%m-%dT%H:%M:%SZ'`
		echo "Restart soap_agent:$time" >> /tmp/restart_process_list
		/usr/sbin/soap_agent &
	fi
	netscan_status=`ps | grep net-scan | grep -v grep | grep -v killall`
	if [ -z "$netscan_status" ];then
		killall -9 net-scan
		time=`date '+%Y-%m-%dT%H:%M:%SZ'`
		echo "Restart net-scan:$time" >> /tmp/restart_process_list
		/usr/sbin/net-scan
	fi
	log_size=`wc -c /tmp/dnsmasq.log |awk '{print $1}'`
	if [ $log_size -gt 1048576 ]; then
		echo -n > /tmp/dnsmasq.log
	fi
done

