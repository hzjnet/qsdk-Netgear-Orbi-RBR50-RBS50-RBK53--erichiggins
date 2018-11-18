# script of examine the Tput for factory mode

orbi_project=`cat /tmp/orbi_project`
wifi_iface=`uci show wireless | grep "\.device=" | awk -F. '{print $2}'`

# wireless interface simple setting for Tput test in factory mode
if [ $1 = "boot"  ]; then 			# implement following function when boot
    for wi in $wifi_iface; do
        wl_athx=`uci -q get wireless.${wi}.ifname`
        backhaul=`uci -q get wireless.${wi}.backhaul`
        if [ "$wl_athx" = "ath0" -o "$wl_athx" = "ath1" -o "$wl_athx" = "ath2" ]; then
            uci set wireless.${wi}.ssid="wl_iface"
            uci set wireless.${wi}.mode=ap
            uci set wireless.${wi}.encryption=none
            uci set wireless.${wi}.key=
	    
            if [ "$backhaul" = "1"  ]; then
                uci set wireless.${wi}.ssid="wl_iface"
                uci set wireless.${wi}.backhaul=0
            fi
	    
            uci set wireless.${wi}.doth=0
            uci set wireless.${wi}.cactimeout=1
        fi

		if [ "$wl_athx" = "ath0" -o "$wl_athx" = "ath01" ]; then
			uci set wireless.${wi}.disablecoext='1'
		fi
    done
fi

# radartool setting for DFS in factory mode
if [ $1 = "post_wlan_up"  ]; then 			# implement following function when wlan up
    if [ "$orbi_project" = "Orbimini"  ]; then
        radartool -i wifi1 usenol 0
    elif [ "$orbi_project" = "Desktop" -o "$orbi_project" = "Orbipro"  ]; then
        radartool -i wifi2 usenol 0
    fi
    
    # disable wifi interface when boot in factory mode
    # and set power to amber to tell now the dut boot is finish
    for wi in $wifi_iface; do
        wl_athx=`uci -q get wireless.${wi}.ifname`
        ifconfig $wl_athx down
    done
    /sbin/ledcontrol -n power -c amber -s on
    
    exit 1
fi

uci commit wireless

