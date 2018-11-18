#!/bin/sh
CONFIG=/bin/config

up_fronthaul ()
{
	wl2g_bh_ap=`config get wl2g_BACKHAUL_AP`
	wl5g_bh_ap=`config get wl5g_BACKHAUL_AP`
	wl2g_bh_sta=`config get wl2g_BACKHAUL_STA`
	wl5g_bh_sta=`config get wl5g_BACKHAUL_STA`
	wifi_iface=`uci show wireless | grep "\.device=" | awk -F. '{print $2}'`

	for wi in $wifi_iface; do
		wl_athx=`uci -q get wireless.${wi}.ifname`
		mode=`uci -q get wireless.${wi}.mode`
		if [ "$wl2g_bh_ap" != "$wl_athx" -a "$wl5g_bh_ap" != "$wl_athx" -a "$wl2g_bh_sta" != "$wl_athx" -a "$wl5g_bh_sta" != "$wl_athx" -a "$mode" = "ap" ]; then
			status=`ifconfig |grep '\<'$wl_athx'\>'`
			if [ "x$status" = "x" ]; then
				ifconfig $wl_athx up
				echo "[$0]Set iface $wl_athx up" >/dev/console
			fi
		fi
	done
}

down_fronthaul ()
{
	wl2g_bh_ap=`config get wl2g_BACKHAUL_AP`
	wl5g_bh_ap=`config get wl5g_BACKHAUL_AP`
	wl2g_bh_sta=`config get wl2g_BACKHAUL_STA`
	wl5g_bh_sta=`config get wl5g_BACKHAUL_STA`
	wifi_iface=`uci show wireless | grep "\.device=" | awk -F. '{print $2}'`
    wl2g_fh_ap=`config get wl2g_NORMAL_AP`
    wl5g_fh_ap=`config get wl5g_NORMAL_AP`
    wl2g_gt_ap=`config get wl2g_GUEST_AP`
    wl5g_gt_ap=`config get wl5g_GUEST_AP`
    wl2g_byod_ap=`config get wl2g_BYOD_AP`
    wl5g_byod_ap=`config get wl5g_BYOD_AP`
    wl2g_arlo_ap=`config get wl2g_ARLO_AP`

	for wi in $wifi_iface; do
		wl_athx=`uci -q get wireless.${wi}.ifname`
		mode=`uci -q get wireless.${wi}.mode`
		if [ "$wl2g_bh_ap" != "$wl_athx" -a "$wl5g_bh_ap" != "$wl_athx" -a "$wl2g_bh_sta" != "$wl_athx" -a "$wl5g_bh_sta" != "$wl_athx" -a "$mode" = "ap" ]; then
			status=`ifconfig |grep '\<'$wl_athx'\>'`
			if [ "x$status" != "x" ]; then
                            if [ "$wl_athx" = "$wl2g_fh_ap" ] || [ "$wl_athx" = "$wl5g_fh_ap" ] || [ "$wl_athx" = "$wl2g_gt_ap" ] || [ "$wl_athx" = "$wl5g_gt_ap" ] || [ "$wl_athx" = "$wl2g_byod_ap" ] || [ "$wl_athx" = "$wl5g_byod_ap" ] || [ "$wl_athx" = "$wl2g_arlo_ap" ]; then
                                echo "[$0]Set iface fronthaul ap down" >/dev/console
		                /usr/sbin/bring_fronthaul_down &
                            fi
			fi
		fi
	done
}

do_check()
{
	orbi_type=`cat /tmp/orbi_type`
	[ "$orbi_type" != "Satellite" ] && exit

	wl2g_backhaul=`config get wl2g_BACKHAUL_STA`
	wl5g_backhaul=`config get wl5g_BACKHAUL_STA`
	wl2g_backhaul_not_connected=`iwconfig $wl2g_backhaul 2>/dev/null | grep "Not-Associated"`
	wl5g_backhaul_not_connected=`iwconfig $wl5g_backhaul 2>/dev/null | grep "Not-Associated"`
	cap_mode=`uci get repacd.repacd.Role`

	if [ "${wl2g_backhaul_not_connected}" -a "${wl5g_backhaul_not_connected}" -a "x$cap_mode" != "xCAP" ]; then
		down_fronthaul
		exit
	fi

	auth_block=`cat /tmp/last_auth_block`
	if [ "x$auth_block" = "x1" -a "x$cap_mode" = "xCAP" ]; then
		down_fronthaul
	else
		up_fronthaul
	fi
}

add_rule()
{
	mac=$1
	[ "x$mac" = "x" ] && exit 0
	mac=`echo ":$mac" | sed 's/:0/:/g' | cut -d ':' -f2-`

	ebtables -t broute -L |grep -i 0x893a |grep -i "$mac" |grep -i DROP > /tmp/deny_rules
	while read loop
	do
		if [ "$loop" != "" ]; then
			rm  /tmp/deny_rules
			 exit 0
		fi
	done < /tmp/deny_rules
	rm  /tmp/deny_rules

	/usr/sbin/ebtables -t broute -A BROUTING -p 0x893a -s $mac -j DROP
}

del_rule()
{
	mac=$1
	[ "x$mac" = "x" ] && exit 0
	[ "x$mac" = "xall" ] && mac=""
	mac=`echo ":$mac" | sed 's/:0/:/g' | cut -d ':' -f2-`
	ebtables -t broute -L | grep -i 0x893a |grep -i "$mac" |grep -i DROP > /tmp/deny_rules
	while read loop
	do
		if [ "$loop" != "" ]; then
			/usr/sbin/ebtables -t broute -D BROUTING $loop
		fi
	done < /tmp/deny_rules
	rm  /tmp/deny_rules
}

port_up()
{
	echo "port $1 up !!!!" >/dev/console
}

case "$1" in
	up)
		port_up $2
	;;
	add)
		add_rule $2
	;;
	del)
		del_rule $2
	;;
	show)
		ebtables -t broute -L
	;;
	check)
		do_check
	;;
	*)
		logger -- "usage: $0 up port_id"
	;;
esac

