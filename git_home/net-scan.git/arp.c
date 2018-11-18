#include "netscan.h"

#ifndef OPENVPN_TUN_DEVICE
#define OPENVPN_TUN_DEVICE
#endif

#ifdef SUPPORT_STREAMBOOST
#define SB_INFO_STATE_1 1
#define SB_INFO_STATE_2 2

typedef enum {
	TYPE_SOAP_OLD = 0, /* This type is for SOAP API */
	TYPE_AMAZON_KINDLE,
	TYPE_ANDROID_DEVICE,
	TYPE_ANDROID_PHONE,
	TYPE_ANDROID_TABLET,
	TYPE_APPLE_AIRPORT_EXPRESS,
	TYPE_BLU_RAY_PLAYER,
	TYPE_BRIDGE,
	TYPE_CABLE_STB,
	TYPE_CAMERA,
	TYPE_ROUTER,
	TYPE_DVR,
	TYPE_GAMING_CONSOLE,
	TYPE_IMAC,
	TYPE_IPAD,
	TYPE_IPAD_MINI,
	TYPE_IPONE_5_5S_5C,
	TYPE_IPHONE,
	TYPE_IPOD_TOUCH,
	TYPE_LINUX_PC,
	TYPE_MAC_MINI,
	TYPE_MAC_PRO,
	TYPE_MAC_BOOK,
	TYPE_MEDIA_DEVICE,
	TYPE_NETWORK_DEVICE,
	TYPE_OTHER_STB,
	TYPE_POWERLINE,
	TYPE_PRINTER,
	TYPE_REPEATER,
	TYPE_SATELLITE_STB,
	TYPE_SCANNER,
	TYPE_SLING_BOX,
	TYPE_SMART_PHONE,
	TYPE_STORAGE_NAS,
	TYPE_SWITCH,
	TYPE_TV,
	TYPE_TABLET,
	TYPE_UNIX_PC,
	TYPE_WINDOWS_PC,
	TYPE_SURFACE,
	TYPE_WIFI_EXTERNDER,
	TYPE_APPLE_TV,
	TYPE_AV_RECEIVER,
	TYPE_CHROMECAST,
	TYPE_GOOGLE_NEXUS_5,
	TYPE_GOOGLE_NEXUS_7,
	TYPE_GOOGLE_NEXUS_10,
	TYPE_OTHERS,
	TYPE_WN1000RP,
	TYPE_WN2500RP,
	TYPE_VOIP,
	TYPE_IPHONE_6_6S,
}netgear_devtype_t;

struct sb2netgear_devtype_mapping_table{
	char *sb_devtype;
	netgear_devtype_t devtype;
};

struct sb2netgear_priority_map_table{
	int  sb_priority;
	char *netgear_priority;
};

struct sb2netgear_priority_map_table priority_map_table[]={
	{10, "HIGHEST"},
	{20, "HIGH"   },
	{30, "MEDIUM" },
	{40, "LOW"    },

	{0 , NULL     }
};

struct device_list{
	char mac[18];
	char priority[8];
	netgear_devtype_t type;
	char host[MAX_HOSTNAME_LEN + 1];
	struct device_list *next;
};

struct sb2netgear_devtype_mapping_table devtype_mapping_table[] = {
	{"Kindle Fire", TYPE_AMAZON_KINDLE},
	{"Kindle Fire HDX", TYPE_AMAZON_KINDLE},
	{"Kindle Fire HD", TYPE_AMAZON_KINDLE},
	{"Android", TYPE_ANDROID_DEVICE},
	{"Samsung Device", TYPE_ANDROID_DEVICE},
	{"HTC Android Device", TYPE_ANDROID_DEVICE},
	{"Droid Mini", TYPE_ANDROID_PHONE},
	{"HTC Amaze 4G", TYPE_ANDROID_PHONE},
	{"HTC Desire", TYPE_ANDROID_PHONE},
	{"HTC Droid DNA", TYPE_ANDROID_PHONE},
	{"HTC J", TYPE_ANDROID_PHONE},
	{"HTC One", TYPE_ANDROID_PHONE},
	{"HTC One S", TYPE_ANDROID_PHONE},
	{"HTC One M8", TYPE_ANDROID_PHONE},
	{"HTC One X", TYPE_ANDROID_PHONE},
	{"HTC Rezound", TYPE_ANDROID_PHONE},
	{"HTC Shift 4G", TYPE_ANDROID_PHONE},
	{"HTC ThunderBolt", TYPE_ANDROID_PHONE},
	{"HomeWizard", TYPE_ANDROID_PHONE},
	{"Samsung Galaxy Nexus", TYPE_ANDROID_PHONE},
	{"Samsung Galaxy Note", TYPE_ANDROID_PHONE},
	{"Samsung Galaxy Note II", TYPE_ANDROID_PHONE},
	{"Samsung Galaxy Note 3", TYPE_ANDROID_PHONE},
	{"Samsung Galaxy S", TYPE_ANDROID_PHONE},
	{"Samsung Galaxy S II", TYPE_ANDROID_PHONE},
	{"Samsung Galaxy S III", TYPE_ANDROID_PHONE},
	{"Samsung Galaxy S4", TYPE_ANDROID_PHONE},
	{"Samsung Galaxy S4 Mini", TYPE_ANDROID_PHONE},
	{"Xperia Z", TYPE_ANDROID_PHONE},
	{"Xperia Zl", TYPE_ANDROID_PHONE},
	{"Xperia Z2", TYPE_ANDROID_PHONE},
	{"Xperia go", TYPE_ANDROID_PHONE},
	{"Moto X", TYPE_ANDROID_PHONE},
	{"LG G2", TYPE_ANDROID_PHONE},
	{"LG G Pro 2", TYPE_ANDROID_PHONE},
	{"Moto G", TYPE_ANDROID_PHONE},
	{"Android Tablet", TYPE_ANDROID_TABLET},
	{"Asus MeMOPad", TYPE_ANDROID_TABLET},
	{"Samsung Galaxy Note Tablet", TYPE_ANDROID_TABLET},
	{"Samsung Galaxy Tab", TYPE_ANDROID_TABLET},
	{"Apple Airport Express", TYPE_APPLE_AIRPORT_EXPRESS},
	{"Sony BD Player", TYPE_BLU_RAY_PLAYER},
	{"Bridge", TYPE_BRIDGE},
	{"Cable STB", TYPE_CABLE_STB},
	{"Camera", TYPE_CAMERA},
	{"D-Link", TYPE_ROUTER},
	{"D-Link DSM-312", TYPE_ROUTER},
	{"Netgear", TYPE_ROUTER},
	{"DVR", TYPE_DVR},
	{"Swann DVR", TYPE_DVR},
	{"Nintendo", TYPE_GAMING_CONSOLE},
	{"Nintendo 3DS", TYPE_GAMING_CONSOLE},
	{"Nintendo Wii", TYPE_GAMING_CONSOLE},
	{"Nintendo Wii U", TYPE_GAMING_CONSOLE},
	{"NVIDIA SHIELD", TYPE_GAMING_CONSOLE},
	{"Playstation", TYPE_GAMING_CONSOLE},
	{"Playstation 3", TYPE_GAMING_CONSOLE},
	{"Playstation 4", TYPE_GAMING_CONSOLE},
	{"Playstation Vita", TYPE_GAMING_CONSOLE},
	{"Xbox One", TYPE_GAMING_CONSOLE},
	{"Xbox 360", TYPE_GAMING_CONSOLE},
	{"iMac", TYPE_IMAC},
	{"iPad", TYPE_IPAD},
	{"iPad 2", TYPE_IPAD},
	{"iPad Air", TYPE_IPAD},
	{"iPad Retina", TYPE_IPAD},
	{"iPad Mini Retina", TYPE_IPAD_MINI},
 	{"iPad Mini", TYPE_IPAD_MINI},
	{"iPhone 5", TYPE_IPONE_5_5S_5C},
	{"iPhone 5c", TYPE_IPONE_5_5S_5C},
	{"iPhone 5s", TYPE_IPONE_5_5S_5C},
	{"iPhone", TYPE_IPHONE},
	{"iPhone 4", TYPE_IPHONE},
	{"iPhone 4s", TYPE_IPHONE},
	{"iPod Touch", TYPE_IPOD_TOUCH},
	{"Linux", TYPE_LINUX_PC},
	{"Mac mini",TYPE_MAC_MINI},
	{"Mac Pro", TYPE_MAC_PRO},
	{"Mac OS X", TYPE_MAC_BOOK},
	{"Roku", TYPE_MEDIA_DEVICE},
	{"SunPower Device", TYPE_MEDIA_DEVICE},
	{"Tivo", TYPE_MEDIA_DEVICE},
	{"Slingbox", TYPE_MEDIA_DEVICE},
	{"Yamaha AVR", TYPE_MEDIA_DEVICE},
	{"Daemon Device", TYPE_MEDIA_DEVICE},
	{"Marantz Device", TYPE_MEDIA_DEVICE},
	{"Sirius XM Radio", TYPE_MEDIA_DEVICE},
	{"Sony Receiver", TYPE_MEDIA_DEVICE},
	{"Demon Device", TYPE_MEDIA_DEVICE},
	{"Cisco", TYPE_NETWORK_DEVICE},
	{"Logitech Media Server", TYPE_NETWORK_DEVICE},
	{"Magicjack", TYPE_NETWORK_DEVICE},
	{"Sonos", TYPE_NETWORK_DEVICE},
	{"Boxee", TYPE_OTHER_STB},
	{"D-Link DSM-312 MovieNite P1", TYPE_OTHER_STB},
	{"HDHomeRun", TYPE_OTHER_STB},
	{"Motorola TV Set-Top", TYPE_OTHER_STB},
	{"Powerline", TYPE_POWERLINE},
	{"HP InKjet", TYPE_PRINTER},
	{"Repeater", TYPE_REPEATER},
	{"Satellite STB", TYPE_SATELLITE_STB},
	{"Scanner", TYPE_SCANNER},
	{"Sling box", TYPE_SLING_BOX},
	{"BlackBerry", TYPE_SMART_PHONE},
	{"BlackBerry 9220", TYPE_SMART_PHONE},
	{"Nokia E72", TYPE_SMART_PHONE},
	{"Nokia Lumia", TYPE_SMART_PHONE},
	{"Nokia Lumia 520", TYPE_SMART_PHONE},
	{"Nokia Lumia 521", TYPE_SMART_PHONE},
	{"Nokia Lumia 822", TYPE_SMART_PHONE},
	{"Nokia Lumia 920", TYPE_SMART_PHONE},
	{"Nokia Lumia 928", TYPE_SMART_PHONE},
	{"Panasonic Smart Device", TYPE_SMART_PHONE},
	{"Symbian Phone", TYPE_SMART_PHONE},
	{"Windows Phone", TYPE_SMART_PHONE},
	{"Droid4", TYPE_SMART_PHONE},
	{"Droid Bionic", TYPE_SMART_PHONE},
	{"Droid M", TYPE_SMART_PHONE},
	{"Droid Razr", TYPE_SMART_PHONE},
	{"Droid Ultra", TYPE_SMART_PHONE},
	{"Storage (NAS)", TYPE_STORAGE_NAS},
	{"Seagate Device", TYPE_STORAGE_NAS},
	{"Synology Device", TYPE_STORAGE_NAS},
	{"Switch", TYPE_SWITCH},
	{"Amazon Fire TV", TYPE_TV},
	{"Google TV", TYPE_TV},
	{"Sharp Aquos LE830", TYPE_TV},
	{"Sharp Aquos TV", TYPE_TV},
	{"Sony KDL-55HX850", TYPE_TV},
	{"Sony TV", TYPE_TV},
	{"Vizio Smart Device", TYPE_TV},
	{"WD TV", TYPE_TV},
	{"DirectTV Device", TYPE_TV},
	{"Netgear NeoTV", TYPE_TV},
	{"Netgear NeoTV NTV300", TYPE_TV},
	{"Netgear NeoTV Prime GTV100", TYPE_TV},
	{"Samsung Smart TV", TYPE_TV},
	{"Sony - KDL-55W900A", TYPE_TV},
	{"Tablet", TYPE_TABLET},
	{"Nook", TYPE_TABLET},
	{"FreeBSD", TYPE_UNIX_PC},
	{"Windows", TYPE_WINDOWS_PC},
	{"Windows RT", TYPE_SURFACE},
	{"Wifi Extender", TYPE_WIFI_EXTERNDER},
	{"Apple TV", TYPE_APPLE_TV},
	{"AV Receiver", TYPE_AV_RECEIVER},
	{"Chromecast", TYPE_CHROMECAST},
	{"Nexus 4", TYPE_GOOGLE_NEXUS_5},
	{"Nexus 5", TYPE_GOOGLE_NEXUS_5},
	{"Nexus 7", TYPE_GOOGLE_NEXUS_7},
	{"Nexus 10", TYPE_GOOGLE_NEXUS_10},
	{"Apple", TYPE_OTHERS},
	{"Kodak Smart Device", TYPE_OTHERS},
	{"La Crosse Technology", TYPE_OTHERS},
	{"Linksys Phone Adapter", TYPE_OTHERS},
	{"Nest", TYPE_OTHERS},
	{"Obihai", TYPE_OTHERS},
	{"Obihai VoIP Adapter", TYPE_OTHERS},
	{"Philips Hue", TYPE_OTHERS},
	{"Unknown", TYPE_OTHERS},
	{"ADEMCO", TYPE_OTHERS},
	{"Chrome OS", TYPE_OTHERS},
	{"Frontier Silicon Radio", TYPE_OTHERS},
	{"HP Device", TYPE_OTHERS},
	{"Vonage Phone Adapter", TYPE_VOIP},
	{"Ooma Telo", TYPE_VOIP},
	{"iPhone 6", TYPE_IPHONE_6_6S},
	{"iPhone 6s", TYPE_IPHONE_6_6S},
	{"iPhone 6s Plus", TYPE_IPHONE_6_6S},
	{NULL, TYPE_OTHERS}
};

inline netgear_devtype_t get_netgear_devtype(char * typename)
{
	int i = 0;
	char *sb_devtype;

	for (sb_devtype = devtype_mapping_table[i].sb_devtype; (sb_devtype = devtype_mapping_table[i].sb_devtype) != NULL; ++i)
		if (strcasecmp(sb_devtype, typename) == 0)
			return devtype_mapping_table[i].devtype;

	return TYPE_OTHERS;
}

int sb_priority_2_netgear_prioriry(int sb_priority, char *netgear_priority)
{
	int i;

	for(i = 0; priority_map_table[i].netgear_priority != NULL; i++){
		if(sb_priority == priority_map_table[i].sb_priority){
			strcpy(netgear_priority, priority_map_table[i].netgear_priority);
			return 0;
		}
	}
	return -1;
}

#define DELETE_LAST_CHAR(str) str[strlen(str) - 1] = 0

static int priority_for_unknow_dev;

void update_streamboost_info(int state, uint8 *mac, struct in_addr ip, char *name, netgear_devtype_t type, double down, double up, long long epoch, int priority);

void get_streamboost_nodes_info(int state)
{
	FILE* fp;

	long long down, up, epoch;
	char mac_s[32];
	struct ether_addr mac;
	char ip_s[128]; /* maybe IPv6 IP address */
	struct in_addr ip;
	char name[512];
	char type_s[128];
	netgear_devtype_t type;
	char default_prio_s[32], priority_s[32];
	int priority = 30; /* MEDIUM = 30 */

	if (state == SB_INFO_STATE_2)
		priority_for_unknow_dev = 0;
	setenv("REQUEST_METHOD", "GET", 1);
	setenv("REQUEST_URI", "/cgi-bin/ozker/api/nodes", 1);
	fp = popen("/usr/bin/cgi-fcgi -bind -connect 127.0.0.1:9000 | sed \'1,2d\' | jq \'.nodes[] | .Pipeline.mac_addr, .Pipeline.ip_addr, .Pipeline.name, .Pipeline.type, .Pipeline.default_prio, .Pipeline.down, .Pipeline.up, .Pipeline.epoch, .UI.priority\'", "r");
	if (fp) {
		while (1) {
			down = 0;
			up = 0;
			epoch = 0;
			memset(mac_s, 0, sizeof(mac_s));
			memset(&mac, 0, sizeof(mac));
			memset(name, 0, sizeof(name));
			memset(type_s, 0, sizeof(type_s));
			memset(priority_s, 0, sizeof(priority_s));
			if (fgets(mac_s, sizeof(mac_s), fp) == NULL)
				break;
			DELETE_LAST_CHAR(mac_s);
			if (ether_aton_r(mac_s, &mac) == NULL) {
				printf("should not run into %s %d\n", __FILE__, __LINE__);
				break;
			}
			if (fgets(ip_s, sizeof(ip_s), fp) == NULL)
				break;
			DELETE_LAST_CHAR(ip_s);
			if (inet_aton(ip_s, &ip) == 0) {
				/* maybe a IPv6 address, we should continue to get rest of node */
				ip.s_addr = 0;
			}
			if (fgets(name, sizeof(name), fp) == NULL) {
				printf("should not run into %s %d\n", __FILE__, __LINE__);
				break;
			}
			DELETE_LAST_CHAR(name);
			if (fgets(type_s, sizeof(type_s), fp) == NULL) {
				printf("should not run into %s %d\n", __FILE__, __LINE__);
				break;
			}
			DELETE_LAST_CHAR(type_s);
			type = get_netgear_devtype(type_s);
			if (fgets(default_prio_s, sizeof(default_prio_s), fp) == NULL) {
				printf("should not run into %s %d\n", __FILE__, __LINE__);
				break;
			}
			DELETE_LAST_CHAR(default_prio_s);
			if (fscanf(fp, "%lld\n", &down) != 1) {
				printf("should not run into %s %d\n", __FILE__, __LINE__);
				break;
			}
			if (fscanf(fp, "%lld\n", &up) != 1) {
				printf("should not run into %s %d\n", __FILE__, __LINE__);
				break;
			}
			if (fscanf(fp, "%lld\n", &epoch) != 1) {
				printf("should not run into %s %d\n", __FILE__, __LINE__);
				break;
			}
			if (fgets(priority_s, sizeof(priority_s), fp) == NULL) {
				printf("should not run into %s %d\n", __FILE__, __LINE__);
				break;
			}
			DELETE_LAST_CHAR(priority_s);
			if (strcmp("null", priority_s) == 0)
				priority = atoi(default_prio_s);
			else
				priority = atoi(priority_s);
			if (ip.s_addr != 0)
				update_streamboost_info(state, mac.ether_addr_octet, ip, name, type, (double)down, (double)up, epoch, priority);
		}
		pclose(fp);
	}
}
void read_device_list(struct device_list *head)
{
	int j;
	char name[16], *val, *mac, *priority, *type, *host;
	struct device_list *p1, *p2;

	head->next = NULL;
	p1 = head;
	for(j = 0; ; j++){
		sprintf(name, "%s%d", "device_list",j);
		val = config_get(name);
		if(*val == '\0')
			break;

		mac = strtok(val, " ");
		priority = strtok(NULL, " ");
		type = strtok(NULL, " ");
		/* Host name can be NULL(means clear the custom name and use the default name )
		 * or includes one or more SPACE characters
		 */
		host = type + strlen(type) + 1;
		if(mac == NULL || priority == NULL || type == NULL)
			continue;

		p2  = malloc(sizeof(struct device_list));
		if(!p2)
			break; /*or make head=NULL and return ?*/

		strncpy(p2->mac, mac, sizeof(p2->mac));
		strncpy(p2->priority, priority, sizeof(p2->priority));
		p2->type = atoi(type);
		strncpy(p2->host, host, sizeof(p2->host));
		p1->next = p2;
		p1       = p2;
		p2->next = NULL;
	}
}
void free_device_list(struct device_list *head)
{
	struct device_list *p1,*p2;

	p1 = head->next;
	while(p1){
		p2 = p1->next;
		free(p1);
		p1 = p2;
	}
}
#endif

struct arp_struct
{
	struct arp_struct *next;

	uint16 active;

	struct in_addr ip;
	uint8 mac[ETH_ALEN];
	ConnectionType ctype;
	unsigned int dtype;
	AttachedType atype;
	char model[MAX_MODEL_LEN + 1];
	char host[MAX_HOSTNAME_LEN + 1];
#ifdef SHOW_PARENT_ROUTER
	char parent[32];
#endif

#ifdef SUPPORT_STREAMBOOST
	uint8 state;
	netgear_devtype_t type;
	double up;
	double down;
	long long epoch;
	int priority;
#endif
};

void update_satellites_and_attachs(struct arp_struct *u);
struct typechar_name t_name;

struct arp_struct *arp_tbl[NEIGH_HASHMASK + 1];

void print_file(char *file)
{
	FILE *fp;
	char buf[256] = {0};
	fp = fopen(file, "r");
	if (fp) {
		DEBUGP("File %s content is:\n", file);
		while(fgets(buf, sizeof(buf), fp))
		{
			DEBUGP("%s", buf);
		}
		fclose(fp);
		DEBUGP("\n");
	}
}

void print_file_info(void)
{
	if(debug_log) {
		print_file("/tmp/netscan/attach_device");
		print_file("/tmp/netscan/satellite_attached_dev");
		print_file("/tmp/netscan_if_conf");
		print_file("/tmp/hyt_result");
		print_file("/tmp/brctl_showmacs_br0");
		print_file("/tmp/device_tables/local_device_table");
		system("config show |grep satellite >> /var/log/netscan/net_scan.log");
		DEBUGP("\n");
		system("iwconfig >> /var/log/netscan/net_scan.log");
		DEBUGP("\n");
		system("(echo \"td s2\";sleep 1) |hyt >> /var/log/netscan/net_scan.log");
		DEBUGP("\n");
	}
}

int get_typechar_name()
{
	FILE *tfp;
	char buff[256],*p;

	if((tfp = fopen("/tmp/netscan_if_typechar_name","r")) == NULL)
	{
		system("touch /tmp/typechar_fail_open"); //fprintf(stderr,"[Net-Scan]Error open typechar_name file!!!\n");
		return -1;
	}

	fgets(buff, sizeof(buff), tfp);
	p = strtok(buff, "-");
	if (p == NULL)
		return -1;
	strlcpy(t_name.wired_name, p, sizeof(t_name.wired_name));
	p = strtok(NULL, "-");
	if (p == NULL)
		return -1;
	strlcpy(t_name.main_2g_name, p, sizeof(t_name.main_2g_name));
	p = strtok(NULL, "-");
	if (p == NULL)
		return -1;
	strlcpy(t_name.main_5g_name, p, sizeof(t_name.main_5g_name));
#ifdef HAVE_GUEST_NETWORK
	p = strtok(NULL, "-");
	if (p == NULL)
		return -1;
	strlcpy(t_name.guest_2g_name, p, sizeof(t_name.guest_2g_name));
	p = strtok(NULL, "-");
	if (p == NULL)
		return -1;
	strlcpy(t_name.guest_5g_name, p, sizeof(t_name.guest_5g_name));
#endif
#ifdef HAVE_GUEST_WIRELESS2
	p = strtok(NULL, "-");
	if (p == NULL)
		return -1;
	strlcpy(t_name.byod_2g_name, p, sizeof(t_name.byod_2g_name));
	p = strtok(NULL, "-");
	if (p == NULL)
		return -1;
	strlcpy(t_name.byod_5g_name, p, sizeof(t_name.byod_5g_name));
#endif
	p = strtok(NULL, "-");
	if (p == NULL)
		return -1;
	strlcpy(t_name.satellite_wired_name, p, sizeof(t_name.satellite_wired_name));
	p = strtok(NULL, "-");
	if (p == NULL)
		return -1;
	strlcpy(t_name.satellite_2g_name, p, sizeof(t_name.satellite_2g_name));
	p = strtok(NULL, "-");
	if (p == NULL)
		return -1;
	strlcpy(t_name.satellite_5g_name, p, sizeof(t_name.satellite_5g_name));

	fclose(tfp);
	return 0;

}

void acl_update_name(uint8 *mac, char *name)
{
	char dev_mac[32];

	dni_system(NULL, "/usr/sbin/acl_update_name", ether_etoa(mac, dev_mac), name, NULL);
}

InterfaceType_t get_ifstr(char *str)
{
	if(!strcmp(str,"wired"))return WIRED_IF;
	else if(!strcmp(str,"2G"))return WIRELESS_2G_IF;
	else if(!strcmp(str,"5G"))return WIRELESS_5G_IF;
	else if(!strcmp(str,"satellite"))return SATELLITE_IF;
#ifdef HAVE_GUEST_NETWORK
	else if(!strcmp(str,"2G-GUEST"))return WIRELESS_2G_GUEST_IF;
	else if(!strcmp(str,"5G-GUEST"))return WIRELESS_5G_GUEST_IF;
#endif
#ifdef HAVE_GUEST_WIRELESS2
	else if(!strcmp(str,"2G-WIRELESS2"))return WIRELESS_2G_WIRELESS2_IF;
	else if(!strcmp(str,"5G-WIRELESS2"))return WIRELESS_5G_WIRELESS2_IF;
#endif
	else return NONETYPE_IF;
}


struct interface_element if_ele[INTERFACENUM];
struct interface_element if_tap = {"tap0", "00:00:00:00:00:00", 0, WIRED_IF};
int interfacenum_max;
int init_interface_info()
{
	FILE *fp;
	int i;
	char buff[128],*ifname,*ctype,*port;

	get_typechar_name();

	if((fp = fopen("/tmp/netscan_if_conf","r")) == NULL)
	{
		fprintf(stderr,"[Net-Scan]Error open if config file!!!\n");
		return -1;
	}
		// Initial Inteface Configure
		for (i = 0; i < INTERFACENUM; i++){
			strncpy(if_ele[i].name, "", sizeof(if_ele[i].name));
			strncpy(if_ele[i].mac, "00:00:00:00:00:00", sizeof(if_ele[i].mac));
			if_ele[i].port = 0;
			if_ele[i].if_type = NONETYPE_IF;
		}
		//Get interface info from file /tmp/netscan_if_conf
		//Format should be like "eth1 wired 1"
		while(fgets(buff, sizeof(buff), fp)){
			ifname = strtok(buff, " ");
			ctype = strtok(NULL, " ");
			port = strtok(NULL, " ");
			if(ifname == NULL || ctype == NULL || port == NULL)
				continue;
			for (i = 0; i < INTERFACENUM; i++){
				if(strcmp(if_ele[i].name, "") == 0) {
					strcpy(if_ele[i].name,ifname);
					if_ele[i].if_type = get_ifstr(ctype);
					if_ele[i].port = atoi(port);
					break;
				}
			}
		}
	fclose(fp);
	return 0;
}

int get_interface_info () {
	FILE *fd;
	int i;
	char buff[512], *port, *bmac;
	
	system("brctl showmacs br0 > /tmp/brctl_showmacs_br0");
	if((fd = fopen("/tmp/brctl_showmacs_br0", "r")) == NULL)
		return -1;

	//get mac addresses to compare
	for (i = 0; i < INTERFACENUM; i++){
		if(if_ele[i].name == NULL) break;
		get_mac(if_ele[i].name, if_ele[i].mac);
	}
	if(config_match("vpn_enable", "1"))
		get_mac(if_tap.name, if_tap.mac);

	/*
	 * #brctl showmacs br0
	 * port no		mac addr			is local?		ageing time
	 * 1			20:14:07:11:2A:20	yes				1.01
	 * ......
	 */

	fgets(buff, sizeof(buff), fd);		//skip first line
	while(fgets(buff, sizeof(buff), fd)){
		port = strtok(buff, " \t\n");
		bmac = strtok(NULL, " \t\n");
		if (port == NULL || bmac == NULL)
			continue;

		strupr(bmac);
		for (i = 0; i < INTERFACENUM; i++){
			if(if_ele[i].name == NULL) break;
			if(!strncmp(bmac, if_ele[i].mac, 17)){
				if_ele[i].port = atoi(port);
				DEBUGP("Found %s interface port id %d\n", if_ele[i].name, if_ele[i].port);
				break;
			}
		}

		if(config_match("vpn_enable", "1"))
			if(!strncmp(bmac, if_tap.mac, 17)){
				if_tap.port = atoi(port);
				DEBUGP("Found %s interface port id %d\n", if_tap.name, if_tap.port);
			}
	}

	//print interface infomation
	DEBUGP("---Interface Table---\n");
	for (i = 0; i < INTERFACENUM; i++){
		if(if_ele[i].name == NULL) break;
		DEBUGP("name:%s MAC:%s port:%d\n", if_ele[i].name, if_ele[i].mac, if_ele[i].port);
	}

	fclose(fd);
	return 0;
}

int get_attach_and_connection_type(struct arp_struct *u, FILE * fd) {
	int i, ret = -1;
	char buff[512], arpmac[32], *port, *bmac, *tap_mac;

	if(fd == NULL) return -1;
	if(u == NULL) return -1;
	fseek(fd, 0, 0); //reset fd pointer to brctl_showmacsbr0 file start
	/*
	 * #brctl showmacs br0
	 * port no		mac addr			is local?		ageing time
	 * 1			20:14:07:11:2A:20	yes				1.01
	 * ......
	 */

	fgets(buff, sizeof(buff), fd);		//skip first line
	ether_etoa(u->mac, arpmac);
	while(fgets(buff, sizeof(buff), fd)){
		int portnum=0;

		port = strtok(buff, " \t\n");
		bmac = strtok(NULL, " \t\n");
		if (port == NULL || bmac == NULL)
			continue;

		strupr(bmac);
		if(strncmp(arpmac, bmac, strlen(arpmac)))
			continue;

		portnum = atoi(port);
		DEBUGP("ONE client[%s] from %d interface.\n", bmac, portnum);
		if(config_match("vpn_enable", "1")) {
			if (if_tap.port == portnum) {
				u->ctype = OPENVPN_TAP;
				u->atype = BASE_ATT;
				return 0;
			}
		}
		for (i = 0; i < INTERFACENUM; i++)
		{
			// For Orbi projects 2.4G is ath0, ath02. 5G is ath1,ath11
			if(strcmp(if_ele[i].name, "") == 0) break;
			if (if_ele[i].port == portnum) {
				switch(if_ele[i].if_type) {
					case WIRED_IF:
						u->ctype = WIRED;
						if (u->atype != BASE_ATT_S)
							u->atype = BASE_ATT;
						update_satellites_and_attachs(u);
						break;
					case WIRELESS_2G_IF:
						u->ctype = WIRELESS_2G;
						u->atype = BASE_ATT;
						break;
#ifdef HAVE_GUEST_NETWORK
					case WIRELESS_2G_GUEST_IF:
						if(config_match("wlg1_endis_guestNet", "1"))
							u->ctype = WIRELESS_2G_GUEST;
#ifdef HAVE_GUEST_WIRELESS2
						else	
						u->ctype = WIRELESS_2G_WIRELESS2;
#endif
						u->atype = BASE_ATT;
						break;
#endif
#ifdef HAVE_GUEST_WIRELESS2
					case WIRELESS_2G_WIRELESS2_IF:
						u->ctype = WIRELESS_2G_WIRELESS2;
						u->atype = BASE_ATT;
						break;
#endif
					case WIRELESS_5G_IF:
						u->ctype = WIRELESS_5G;
						u->atype = BASE_ATT;
						break;
#ifdef HAVE_GUEST_NETWORK
					case WIRELESS_5G_GUEST_IF:
						if(config_match("wlg1_endis_guestNet", "1"))
							u->ctype = WIRELESS_5G_GUEST;
#ifdef HAVE_GUEST_WIRELESS2
						else
							u->ctype = WIRELESS_5G_WIRELESS2;
#endif
						u->atype = BASE_ATT;
						break;
#endif
#ifdef HAVE_GUEST_WIRELESS2
					case WIRELESS_5G_WIRELESS2_IF:
						u->ctype = WIRELESS_5G_WIRELESS2;
						u->atype = BASE_ATT;
						break;
#endif
					case SATELLITE_IF:
						update_satellites_and_attachs(u);
						break;
					default:
						break;
				}
				ret = 0;
				break;
			}
		}
		break;
	}
	DEBUGP("ONE client[%s] connection type is:%d, connected orbi is:%d.\n", bmac, u->ctype, u->atype);

	return ret;
}

#ifdef HAVE_IGNORE_TABLE
/*Don't need to save some devices into devices table and should not show them on the Attached Devices Page.
 * e.g. Arlo Cameras
 * */
struct ignore_table *ignore_tbl[NEIGH_HASHMASK + 1];

void update_ignore_table(void)
{
	struct ignore_table *u;
	struct ether_addr *mac_addr;
	FILE *fp;
	int key;
	char line[64], *mac, *ip;

	if ((fp = fopen(ARLO_LIST, "r")) == NULL)
		return;
	while (fgets(line, sizeof(line), fp)) {
		mac = strtok(line, ",");
		ip  = strtok(NULL, "\t\n");
		if (mac == NULL || ip == NULL)
			continue;
		mac_addr = ether_aton(mac);
		if (mac_addr == NULL)
			continue;
		key = arp_hash(mac_addr->ether_addr_octet);

		for (u = ignore_tbl[key]; u && memcmp(u->mac, mac_addr, ETH_ALEN); u = u->next);
		if (u) {
			inet_aton(ip, &u->ip);
		}
		else {
			u = malloc(sizeof(struct ignore_table));
			if (u == 0)
				continue;
			memset(u, 0, sizeof(struct ignore_table));
			inet_aton(ip, &u->ip);
			memcpy(u->mac, mac_addr, ETH_ALEN);
			u->next = ignore_tbl[key];
			ignore_tbl[key] = u;
		}
	}
	fclose(fp);
	return;
}

static int ignore_mac(uint8 *mac)
{
	struct ignore_table *u;
	int key;

	if (mac == NULL)
		return 0;

	key = arp_hash(mac);
	for (u = ignore_tbl[key]; u && memcmp(u->mac, mac, ETH_ALEN); u = u->next);
	if (u) {
		return 1;
	}
	return 0;
}

static int ignore_ip(struct in_addr ip)
{
	struct ignore_table *u;
	int i;

	for (i = 0; i < (NEIGH_HASHMASK + 1); i++) {
		for (u = ignore_tbl[i]; u; u = u->next) {
			if (u->ip.s_addr == ip.s_addr) {
				return 1;
			}
		}
	}
	return 0;
}
#endif

static int total_dev = 0;

int update_arp_table(uint8 *mac, struct in_addr ip, int isrepl)
{
	uint32 i;
	FILE *brfd = NULL;
	char host[MAX_HOSTNAME_LEN + 1] = {0}, bmac[32] = {0};
	struct arp_struct *u;

#ifdef HAVE_IGNORE_TABLE
	if (ignore_mac(mac))
		return 0;
#endif
	/* check dhcp host */
	get_dhcp_host(host, ip, &isrepl);
	i = arp_hash(mac);
	if(i > NEIGH_HASHMASK)
	{
		DEBUGP("Receive a error ARP packet: HASH_VALUE:%d", i);
		return 0;
	}
	ether_etoa(mac, bmac);
	if (*host)
		DEBUGP("Receive a ARP packet: HASH_VALUE:%d IP:%s HOST:%s MAC:%s\n", i, inet_ntoa(ip), host, bmac);
	else
		DEBUGP("Receive a ARP packet: HASH_VALUE:%d IP:%s MAC:%s\n", i, inet_ntoa(ip), bmac);

	for (u = arp_tbl[i]; u && memcmp(u->mac, mac, ETH_ALEN); u = u->next);
	if (u) {
		if (*host) {
			strncpy(u->host, host, MAX_HOSTNAME_LEN);
			acl_update_name(u->mac, host);
		}
		u->ip = ip;              /* The IP may be changed for DHCP      */
		u->active = 1;

#ifdef SUPPORT_TRENDMICRO
		config_set("qos_priority_set","1");
#endif

		return isrepl;	/* Do BIOS Name Query only ARP reply */
	}

	u = malloc(sizeof(struct arp_struct));
	if (u == 0)
		return 0;
	memset(u, 0, sizeof(struct arp_struct));
	u->ip = ip;
	u->active = 1;
	memcpy(u->mac, mac, ETH_ALEN);
	total_dev++;

	/* start catch HTTP packets when new mac is found*/
	if(strcmp(config_get("support_trend_micro_qos"),"1") != 0)
	system("/usr/sbin/UA_Parser");

	if (*host) {
		strncpy(u->host, host, MAX_HOSTNAME_LEN);
		acl_update_name(u->mac, host);
	}

	u->ctype = NONETYPE; //default value
	u->atype = NONETYPE_ATT;
	get_interface_info();
	if((brfd = fopen("/tmp/brctl_showmacs_br0", "r")) != NULL) {
		DEBUGP("cannot open /tmp/brctl_showmacs_br0\n");
		get_attach_and_connection_type(u, brfd);
		fclose(brfd);
	}
	u->dtype = DEFAULT_DEVICE_TYPE;
	get_model_name(u->mac, u->model);

	printf("There is a new device coming, IP:%s HOST:%s MAC:%s Device Type:%d Model:%s Connection Type:%d Connected Orbi:%d Parent:%s\n",\ 
			inet_ntoa(u->ip), host, bmac, u->dtype, u->model, u->ctype, u->atype, u->parent);
/*
 *	update local device table
 *	run UA_Parser
 */
#ifdef SUPPORT_STREAMBOOST
	u->type = TYPE_OTHERS;
#endif

	u->next = arp_tbl[i];
	arp_tbl[i] = u;

	return isrepl;
}

static void update_name(struct in_addr ip, char *host)
{
	int i;
	struct arp_struct *u;

	for (i = 0; i < (NEIGH_HASHMASK + 1); i++) {
		for (u = arp_tbl[i]; u; u = u->next)
			if (u->ip.s_addr == ip.s_addr) {
				strncpy(u->host, host, MAX_HOSTNAME_LEN);
				acl_update_name(u->mac, host);
				return;
			}
	}
}

void update_bios_name(uint8 *mac, char *host, struct in_addr ip)
{
	uint32 i;
	struct arp_struct *u;

	i = arp_hash(mac);
	for (u = arp_tbl[i]; u && memcmp(u->mac, mac, ETH_ALEN); u = u->next);

	if (u == 0) {
		update_name(ip, host); /* try it by IP address */
		return;
	}

	strncpy(u->host, host, MAX_HOSTNAME_LEN);
	acl_update_name(u->mac, host);
}

#ifdef SUPPORT_STREAMBOOST
void update_streamboost_info(int state, uint8 *mac, struct in_addr ip, char *name, netgear_devtype_t type, double down, double up, long long epoch, int priority)
{
	uint32 i;
	struct arp_struct *u;

	i = arp_hash(mac);
	for (u = arp_tbl[i]; u && memcmp(u->mac, mac, ETH_ALEN); u = u->next); /* or find the node according ip ? */

	if (state == SB_INFO_STATE_2 && u && u->active == 1 && priority > priority_for_unknow_dev)
		priority_for_unknow_dev = priority;

	if (!u) {
		u = malloc(sizeof(struct arp_struct));
		if (!u)
			return;
		memset(u,  0, sizeof(struct arp_struct));
		u->ip = ip;
		memcpy(u->mac, mac, ETH_ALEN);
		u->next = arp_tbl[i];
		arp_tbl[i] = u;
	}

	if (strcmp(name, "null") != 0) {
		strncpy(u->host, name, MAX_HOSTNAME_LEN);
		acl_update_name(u->mac, name); /* should update acl name here ? */
	}
	u->type = type;
	u->priority = priority;

	if (state == SB_INFO_STATE_1) {
		u->down = down;
		u->up = up;
		u->epoch = epoch;
		u->state = state;
	}
	else if (state == SB_INFO_STATE_2 && u->state == SB_INFO_STATE_1) {
		long long divisor;
		divisor = (epoch - u->epoch) * 1000000 / 8;
		u->down = (down - u->down) / divisor;
		u->up = (up - u->up) / divisor;
		if (u->down < 0.0)
			u->down = 0.0;
		if (u->up < 0.0)
			u->up = 0.0;
		u->state = SB_INFO_STATE_2;
	}
}
#endif

void recv_bios_pack(char *buf, int len, struct in_addr from)
{
#define HDR_SIZE		sizeof(struct nb_response_header)
	uint16 num;
	uint8 *p, *e;
	struct nb_response_header *resp;

	if (len < HDR_SIZE)
		return;

	resp = (struct nb_response_header *)buf;
	num = resp->num_names;
	p = (uint8*)&buf[HDR_SIZE];
	e = p + (num * 18);
	/* unique name, workstation service - this is computer name */
	for (; p < e; p += 18)
		if ((p[15] == 0 || p[15] == 0x20) && (p[16] & 0x80) == 0)
			break;
	if (p == e)
		return;
	if (p[15] == 0x20)
		p[15] = 0;
	update_bios_name(e, (char *)p, from);
}

int open_arp_socket(struct sockaddr *me)
{
	int s;
	int buffersize = 200 * 1024;

	s = socket(PF_PACKET, SOCK_PACKET, htons(ETH_P_ARP));
	if (s < 0)
		goto err;

	/* We're trying to override buffer size  to set a bigger buffer. */
	if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize)))
		DEBUGP("setsocketopt error!\n");

	me->sa_family = PF_PACKET;
	strncpy(me->sa_data, ARP_IFNAME, 14);

	if (bind(s, me, sizeof(*me)) < 0)
		goto err;	

	return s;
err:
	if(s >= 0)
		close(s);
	return -1;
}

int recv_arp_pack(struct arpmsg *arpkt, struct in_addr *send_ip)
{
	static uint8 zero[6] = { 0, 0, 0, 0, 0, 0 };

	struct in_addr src_ip;

	if (arpkt->ar_op != htons(ARPOP_REQUEST) && arpkt->ar_op != htons(ARPOP_REPLY))
		return 0;
	if (arpkt->ar_hrd != htons(ARPHRD_ETHER) ||arpkt->ar_pro != htons(ETH_P_IP))
		return 0;
	if (arpkt->ar_pln != 4 ||arpkt->ar_hln != ETH_ALEN)
		return 0;

	/*
	  * If It is Gratuitous ARP message, ignore it for Home Router passing Xbox test,
	  * else we need change the `udhcpd` code about `checking IP used` too much
	  * to pass `XBox DHCP Lease Test`. The normal ARP message ==MAY BE== all
	  * right for Attached Devices function.... &_&.
	  */
	if (memcmp(arpkt->ar_sip, arpkt->ar_tip, 4) == 0)
		return 0;

	memcpy(&src_ip, arpkt->ar_sip, 4);
	if (src_ip.s_addr == 0 ||memcmp(arpkt->ar_sha, zero, 6) == 0)
		return 0;

	*send_ip = src_ip;
	return update_arp_table(arpkt->ar_sha, src_ip, arpkt->ar_op == htons(ARPOP_REPLY));
}

void update_satellite_name(struct arp_struct *u) {
	int scount, i;
	cJSON *dataArray, *Item, *macItem, *modelItem, *deviceItem;
	char mac[32];

	system("/usr/sbin/satelliteinfo device > /tmp/netscan/current_satellite_list");
	ether_etoa(u->mac, mac);

	if((dataArray = getJSONcode("/tmp/netscan/current_satellite_list")) != NULL) {
		scount = cJSON_GetArraySize(dataArray);

		for(i=0; i<scount; i++) {
			Item  = cJSON_GetArrayItem(dataArray, i);
			if (Item == NULL) goto err;
			macItem = cJSON_GetObjectItem(Item, "mac address");
			modelItem = cJSON_GetObjectItem(Item, "module name");
			deviceItem =  cJSON_GetObjectItem(Item, "device name");
			if (macItem == NULL || modelItem == NULL || deviceItem == NULL) goto err;
			if (strcmp(macItem->valuestring, mac) == 0) {
				strncpy(u->host, deviceItem->valuestring, MAX_HOSTNAME_LEN);
				DEBUGP("Get satellite[MAC:%s] name:%s\n", mac, u->host);
				cJSON_Delete(dataArray);
				return;
			}
		}

	}

	DEBUGP("[net-scan]name of satellite[MAC:%s] is not found\n", mac);
	cJSON_Delete(dataArray);
	return;
err:
	DEBUGP("[net-scan]Get satelliteinfo device error\n");
	cJSON_Delete(dataArray);
	return;
}

#ifdef SHOW_PARENT_ROUTER
void update_satellite_default_name(struct arp_struct *u, char *satellite_mac) {
	char *val, *mac_t, name_t[32], item[32];
	int i;
	for(i =0; ; i++) {
		sprintf(item, "%s%d", "satellite", i);
		val = config_get(item);
		if (*val == '\0')
			break;
		mac_t = strtok(val, " ");
		if(strcasecmp(satellite_mac, mac_t) == 0) {
			sprintf(name_t, "Orbi Satellite-%d", i+1);
			strncpy(u->host, name_t, sizeof(u->host));
			break;
		}
	}
	return;
}
#endif

void update_satellite_attaches(void) {
	struct arp_struct *u;
	struct ether_addr *mac_addr;
	int scount, i, dcount, j, k;
	cJSON *dataArray, *Item, *macItem, *typeItem, *deviceItem, *satellite_dev, *sItem;
	char mac[32], type[32], *p0, *sname;
	memset(type, 0, sizeof(type));

	system("/usr/sbin/satelliteinfo attached > /tmp/netscan/satellite_attached_dev");
	if((dataArray = getJSONcode("/tmp/netscan/satellite_attached_dev")) != NULL) {
		scount = cJSON_GetArraySize(dataArray);

		for(i=0; i<scount; i++) {
			satellite_dev = cJSON_GetArrayItem(dataArray, i);
			p0 = cJSON_PrintUnformatted(satellite_dev);
			strtok(p0, "\"");
			sname = strtok(NULL, "\"");
			sItem = cJSON_GetObjectItem(satellite_dev, sname);
			if (sItem == NULL)
				goto err;

			dcount = cJSON_GetArraySize(sItem);
			for(j=0; j<dcount; j++) {
				Item  = cJSON_GetArrayItem(sItem, j);
				macItem = cJSON_GetObjectItem(Item, "attached device mac address");
				typeItem = cJSON_GetObjectItem(Item, "attached connection type");
				deviceItem =  cJSON_GetObjectItem(Item, "attached device name");

				if (macItem == NULL || typeItem == NULL || deviceItem == NULL)
					goto err;

				mac_addr = ether_aton(macItem->valuestring);
				if (mac_addr == NULL)
					goto err;
				k = arp_hash(mac_addr->ether_addr_octet);
				for (u = arp_tbl[k]; u && memcmp(u->mac, &mac_addr->ether_addr_octet, ETH_ALEN); u = u->next);

				if (u)
					ether_etoa(u->mac, mac);

				if (strcmp(macItem->valuestring, mac) == 0) {
					if (u->atype != BASE_ATT_S)
						u->atype = SATELLITE_ATT;
#ifdef SHOW_PARENT_ROUTER
					strncpy(u->parent, sItem->string, sizeof(u->parent));
					if(u->parent[0] == '\0' && u->atype == BASE_ATT)
						strncpy(u->parent, "Orbi Router", sizeof(u->parent));
#endif
					if (u->host[0] == '\0')
						strncpy(u->host, deviceItem->valuestring, MAX_HOSTNAME_LEN);
					strncpy(type, typeItem->valuestring, 32);

					if (strncmp(type, t_name.wired_name, strlen(type)) == 0)
						u->ctype = WIRED;
					else if (strncmp(type, t_name.main_2g_name, strlen(type)) == 0)
						u->ctype = WIRELESS_2G;
					else if (strncmp(type, t_name.main_5g_name, strlen(type)) == 0)
						u->ctype = WIRELESS_5G;	
#ifdef HAVE_GUEST_NETWORK
					else if (strncmp(type, t_name.guest_2g_name, strlen(type)) == 0)
						u->ctype = WIRELESS_2G_GUEST;
					else if (strncmp(type, t_name.guest_5g_name, strlen(type)) == 0)
						u->ctype = WIRELESS_5G_GUEST;
#endif
#ifdef HAVE_GUEST_WIRELESS2
					else if (strncmp(type, t_name.byod_2g_name, strlen(type)) == 0)
						u->ctype = WIRELESS_2G_WIRELESS2;
					else if (strncmp(type, t_name.byod_5g_name, strlen(type)) == 0)
						u->ctype = WIRELESS_5G_WIRELESS2;
#endif
					else if (strcmp(type, "2.4G") == 0) //type for WallPlug 2.4G backhaul
						u->ctype = WIRELESS_2G;
					else if (strcmp(type, "5G") == 0) //type for WallPlug 5G backhaul
						u->ctype = WIRELESS_5G;
				}
			}
			if(p0) {
				free(p0);
				p0 = NULL;
			}
		}

	}

	if(p0) {
		free(p0);
		p0 = NULL;
	}
	cJSON_Delete(dataArray);
	return;
err:
	DEBUGP("[net-scan]Get satelliteinfo attached device error\n");
	if(p0) {
		free(p0);
		p0 = NULL;
	}
	cJSON_Delete(dataArray);
	return;

}

#ifdef SHOW_PARENT_ROUTER
static void init_devices_parent(int count, struct arp_struct *u, FILE *fp_orbi) {
	char wlan_line[256], *tmp, *orbimac;

	if (count == 1 && fp_orbi != NULL) {
		fseek(fp_orbi, 0, 0);
		fgets(wlan_line, sizeof(wlan_line), fp_orbi);
		tmp = wlan_line;
		orbimac = strtok(tmp, ",");
		if (orbimac != NULL)
			strncpy(u->parent, orbimac, sizeof(u->parent));
	}
	else
		strncpy(u->parent, "Orbi Satellite", sizeof(u->parent));

	return;
}
#endif

void update_satellites_and_attachs(struct arp_struct *u) {
	FILE *fp_orbi = NULL;
	char name[64], cfg_change[128];
	char *val, *sep;
	char *status_t, *mac_t, *type_t, *order_t, *host_t, *log_t;
	int i, count = 0;
	char mac[32], wlan_line[256], *tmp, *orbimac, *orbiip;
	ether_etoa(u->mac, mac);

	sep = " ";

	fp_orbi = fopen("/tmp/hyt_result", "r");
	if (fp_orbi != NULL) {
		while(fgets(wlan_line, sizeof(wlan_line), fp_orbi)) {
			tmp = wlan_line;
			orbimac = strtok(tmp, ",");
			orbiip = strtok(NULL, ",");
			if (orbimac == NULL || orbiip == NULL)
				continue;
			count++;
			if (strcmp(orbimac, mac) == 0) {
				if (config_match("ap_mode", "1"))
					update_satellite_name(u);
				inet_aton(orbiip, &u->ip);
				u->atype = BASE_ATT_S;
				if (u->ctype == NONETYPE)
					u->ctype = SATELLITE;
				u->dtype = 0;//The number of the GUI icon for satellite
				strcpy(u->model, "Netgear");
				strcpy(u->parent, "Unknown");

#ifdef SHOW_PARENT_ROUTER
				update_satellite_default_name(u, mac);
#endif
				for ( i = 1; ; i++)
				{
					sprintf(name, "%s%d", "access_control", i);
					val = config_get(name);

					if (*val == '\0')
						break;

					status_t = strtok(val, sep);
					mac_t = strtok(NULL, sep);
					type_t = strtok(NULL, sep);
					order_t = strtok(NULL, sep);
					host_t = strtok(NULL, sep);
					log_t = strtok(NULL, sep);
					if(strcasecmp(mac_t, mac) == 0 && strcmp(type_t, "3") != 0)
					{
						snprintf(cfg_change, sizeof(cfg_change)-1, "%s %s %s %s %s %s", status_t, mac_t, "3", order_t, host_t, log_t);
						cfg_change[sizeof(cfg_change)-1] = '\0';
						config_set(name, cfg_change);
						break;
					}
				}
				fclose(fp_orbi);
				return;
			}
		}
	}
#ifdef SHOW_PARENT_ROUTER
	if (u->atype == NONETYPE_ATT || u->atype == SATELLITE_ATT)
		init_devices_parent(count, u, fp_orbi);
#endif
	if(u->atype != BASE_ATT_S && u->atype != BASE_ATT)
		u->atype = SATELLITE_ATT;

	if(fp_orbi)
		fclose(fp_orbi);
	return;
}

int if_use_default_value(const char *mac, const char *name)
{
	char item[32], *val, *mac_t, *type_t, *model_t;
	int i;
	memset(item, 0, sizeof(item));
	for (i = 1;; i++) {
		snprintf(item, sizeof(item)-1, "%s%d", name, i);
		val = config_get(item);
		if (*val == '\0')
			break;

		mac_t = strtok(val, " ");
		if (strcmp(mac, mac_t) == 0) {
			if (strcmp(name,"orbi_dev_name") == 0) {
					return 3;
			}
			else if (strcmp(name, "orbi_dev_name_ntgr") == 0) {
				type_t = strtok(NULL, " ");
				model_t = strtok(NULL, "\n");
				if (strcmp(type_t, "0") == 0 && model_t == NULL)
					return 0;
				else if (strcmp(type_t, "0") == 0)
					return 1;
				else if (model_t == NULL)
					return 2;
				else
					return 3;
			}
		}

	}
	return 0;
}

void update_devices_info(struct arp_struct *u) {
	FILE *fp,*tfp;
	char line[256], arpmac[32], *newline, *mac, *type, *model, *name, *flag, *type_s, *model_s, *name_s;
	int i,find = 0;
	char cmd[512];

	memset(cmd,0,sizeof(cmd));
	fp = fopen(LOCAL_DEVICE_FILE, "r");
	if (fp == NULL) return;

	ether_etoa(u->mac, arpmac);
	while (fgets(line, sizeof(line), fp)) {
		newline = strtok(line, "\n");
		mac = strtok(newline, ",");
		if (strcmp(mac, arpmac) == 0) {
			find = 1;
			type = strtok(NULL, ",");
			model = strtok(NULL, ",");
			name = strtok(NULL, ",");
			flag = strtok(NULL, ",");
			type_s = strtok(NULL, ",");
			model_s = strtok(NULL, ",");
			name_s = strtok(NULL, ",");
			if(type == NULL || model == NULL || name == NULL || flag == NULL || type_s == NULL || model_s == NULL || name_s == NULL)
				continue;

			i = vendor_filter(model);
			if (strcmp(flag, "1") == 0) {	//user customized device
				if ((i >= 0) && (vendor_table[i].vendor_m != NULL))
					strncpy(model, vendor_table[i].vendor_m, MAX_MODEL_LEN);
					if (if_use_default_value(arpmac, "orbi_dev_name_ntgr") == 0) {
						u->dtype = atoi(type_s);
						strncpy(u->model, model_s, MAX_MODEL_LEN);
					}
					else if (if_use_default_value(arpmac, "orbi_dev_name_ntgr") == 1)
						u->dtype = atoi(type_s);
					else if (if_use_default_value(arpmac, "orbi_dev_name_ntgr") == 2)
						strncpy(u->model, model_s, MAX_MODEL_LEN);
					else {
						u->dtype = atoi(type);
						strncpy(u->model, model, MAX_MODEL_LEN);
					}
					if (if_use_default_value(arpmac, "orbi_dev_name") == 0)
						strncpy(u->host, name_s, MAX_HOSTNAME_LEN);
					else
						strncpy(u->host, name, MAX_HOSTNAME_LEN);
			} else {
				find = 2;
				if ((i >= 0) && (vendor_table[i].vendor_m != NULL))
					strncpy(model, vendor_table[i].vendor_m, MAX_MODEL_LEN);
				strncpy(u->model, model, MAX_MODEL_LEN);
				u->dtype = atoi(type);
			}
			break;
		}
	}

	fclose(fp);

	if(strcmp(config_get("support_trend_micro_qos"),"1") == 0){
	if(find == 0 || find == 2){
		char *tm_ip,*tm_mac,*tm_type_str;
		int tm_type = 0;
		tfp = fopen("/tmp/netscan/attach_device_trend_micro", "r");
		while (fgets(line, sizeof(line), tfp)) {
			newline = strtok(line, "\n");
			tm_ip = strtok(newline, " ");
			tm_mac = strtok(NULL, " ");
			if (strcmp(tm_mac, arpmac) == 0) {
				tm_type_str = strtok(NULL, " ");
				tm_type = atoi(tm_type_str);
				if(tm_type && tm_type <= 47){
					u->dtype = tm_type;
					if(strcmp(u->model,"") == 0 || strcmp(u->host,"") == 0)continue;
					if(find == 2){
						if(strcmp(type,tm_type_str) != 0){
							snprintf(cmd, sizeof(cmd), "devices_info update \"%s\" \"%s\" \"%s\" \"%s\"", arpmac, tm_type_str, u->model, u->host);
							system(cmd);
							break;
						}
					}
					else{
						fp = fopen(LOCAL_DEVICE_FILE, "a+");
						fprintf(fp, "%s,%s,%s,%s,%d,%s,%s,%s\n", arpmac, tm_type_str, u->model, u->host, 0, tm_type_str, u->model, u->host);
						fclose(fp);
						break;
					}
				}
			}
		}
		fclose(tfp);
	}
	}

	return;
}

void get_distant_satellite_connection_type(struct arp_struct *u)
{
	FILE *fp;
	char mac[32], wlan_line[256];

	ether_etoa(u->mac, mac);

	fp = fopen("/tmp/hyt_distant_result", "r");
	if(fp == NULL)
		return;

	while(fgets(wlan_line, sizeof(wlan_line), fp)) {
		if (strstr(wlan_line, mac)) {
			if (strstr(wlan_line, "WLAN5G"))
				u->ctype = SATELLITE_5G;
			else if (strstr(wlan_line, "WLAN2G"))
				u->ctype = SATELLITE_2G;
			else if (strstr(wlan_line, "ETHER"))
				u->ctype = SATELLITE_WIRED;
			strcpy(u->parent, "Unknown");
			break;
		}
	}
	fclose(fp);
	return;
}

void get_satellite_connection_type(struct arp_struct *u)
{
	FILE *fp;
	char mac[32], wlan_line[256];

	ether_etoa(u->mac, mac);

	fp = fopen("/tmp/hyt_result", "r");
	if(fp == NULL)
		return;

	while(fgets(wlan_line, sizeof(wlan_line), fp)) {
		if (strstr(wlan_line, mac)) {
			if (strstr(wlan_line, "WLAN5G"))
				u->ctype = SATELLITE_5G;
			else if (strstr(wlan_line, "WLAN2G"))
				u->ctype = SATELLITE_2G;
			else if (strstr(wlan_line, "ETHER"))
				u->ctype = SATELLITE_WIRED;
			else {
				get_distant_satellite_connection_type(u);
				if (u->ctype != SATELLITE_5G && u->ctype != SATELLITE_2G && u->ctype != SATELLITE_WIRED)
					u->ctype = SATELLITE;
			}
			DEBUGP("ONE client[%s] is satellite, connection type is:%d.\n", mac, u->ctype);
			break;
		}
	}
	fclose(fp);
	return;
}

#ifdef OPENVPN_TUN_DEVICE
int vpntun_get_arpmac(char *ip, char *mac)
{
	FILE *mac_fp;
	char mac_cmd[64] = {0}, vpntun_mac[40] = {0};
	sprintf(mac_cmd, "arping -w 1 -f -I brwan %s |awk -F \" \" \'{print $5}\'", ip);
	mac_fp = popen(mac_cmd, "r");
	if (mac_fp) {
		while(fgets(vpntun_mac, sizeof(vpntun_mac), mac_fp)) {
			if (strstr(vpntun_mac, "[")) {
				char *mac_head = strstr(vpntun_mac, "[") + 1;
				char *mac_end = strstr(vpntun_mac, "]");
				strncpy(mac, mac_head, mac_end - mac_head);
				pclose(mac_fp);
				return 1;
			}
		}
		pclose(mac_fp);
	}
	return 0;
}
#endif

void show_arp_table(void)
{
	int i, device_num = 1, satellite_num = 0;
	unsigned int mac_num = 0;
	FILE *fp, *hyt_fp;
	FILE *brfd = NULL;
	char mac[32];
	struct arp_struct *u;
	struct arp_struct **pprev;
#ifdef SUPPORT_STREAMBOOST
	FILE *sb_fp, *name_type_fp;
	struct device_list head_node, *sb_device_list, *sb_dlp;
	char  netgear_priority[32];

	get_streamboost_nodes_info(SB_INFO_STATE_2);
	sb_device_list = &head_node;
	read_device_list(sb_device_list);
	sb_fp = fopen("/tmp/netscan/attach_device_streamboost", "w");
	name_type_fp = fopen("/tmp/netscan/default_name_type", "w");
#endif
#ifdef OPENVPN_TUN_DEVICE
	FILE *openvpn_tun_fp;
	openvpn_tun_fp = fopen("/tmp/openvpnd_tun.status", "r");
#endif
	get_interface_info();
	if((brfd = fopen("/tmp/brctl_showmacs_br0", "r"))) {
		for (i = 0; i < (NEIGH_HASHMASK + 1); i++) {
			for (pprev = &arp_tbl[i], u = *pprev; u; ) {
				get_attach_and_connection_type(u, brfd);
				pprev = &u->next;
				u = u->next;
			}
		}
		fclose(brfd);
	}
	else
		DEBUGP("cannot open /tmp/brctl_showmacs_br0\n");

#ifdef SUPPORT_CIRCLE
	int circle_writen = 0;
	FILE *cfp = NULL;
	if(config_match("enable_circle_plc","1") && config_match("ap_mode","0") && config_match("save_circle_info","1")){
		cfp = fopen("/tmp/AttachDevice", "w");
		circle_writen = 1;
		fprintf(cfp, "<NewAttachDevice>\r\n");
	}
#endif
	fp = fopen(ARP_FILE, "w");
	hyt_fp = fopen("/tmp/soapclient/allconfig_result", "r");
	if (fp == NULL) {
		DEBUGP("[net-scan]open %s failed\n", ARP_FILE);
	}

	update_satellite_attaches();

	for (i = 0; i < (NEIGH_HASHMASK + 1); i++) {
		char typechar[32] = {0}, bmac[32] = {0};
		for (pprev = &arp_tbl[i], u = *pprev; u; ) {
			mac_num++;
			if (u->active == 0) {
				DEBUGP("No Active, don't display: [%s] [%s]\n", ether_etoa(u->mac, mac), inet_ntoa(u->ip));
				if(mac_num > 255 && u->atype != BASE_ATT_S) {
					ether_etoa(u->mac, bmac);
					printf("There is a device delete from arp table, IP:%s, MAC:%s\n", inet_ntoa(u->ip), bmac);
					*pprev = u->next;
					free(u);
					u = *pprev;
					total_dev--;
				}
				else {
					pprev = &u->next;
					u = u->next;
				}
				continue;
			}

#ifdef SHOW_PARENT_ROUTER
			if(u->atype == BASE_ATT)
				strncpy(u->parent, "Orbi Router", sizeof(u->parent));
#endif
			if (u->atype == BASE_ATT_S)
				get_satellite_connection_type(u);

			switch (u->ctype) {
				case WIRELESS_2G:
					strcpy(typechar, t_name.main_2g_name);
					break;
				case WIRELESS_5G:
					strcpy(typechar, t_name.main_5g_name);
					break;
#ifdef HAVE_GUEST_NETWORK
				case WIRELESS_2G_GUEST:
					strcpy(typechar, t_name.guest_2g_name);
					break;
				case WIRELESS_5G_GUEST:
					strcpy(typechar, t_name.guest_5g_name);
					break;
#endif
#ifdef HAVE_GUEST_WIRELESS2
				case WIRELESS_2G_WIRELESS2:
					strcpy(typechar, t_name.byod_2g_name);
					break;
				case WIRELESS_5G_WIRELESS2:
					strcpy(typechar, t_name.byod_5g_name);
					break;
#endif
				case SATELLITE:
					get_satellite_connection_type(u);
					update_satellites_and_attachs(u);
					satellite_num++;
					break;
				case SATELLITE_2G:
					strcpy(typechar, t_name.satellite_2g_name);
					satellite_num++;
					break;
				case SATELLITE_5G:
					strcpy(typechar, t_name.satellite_5g_name);
					satellite_num++;
					break;
				case SATELLITE_WIRED:
					strcpy(typechar, t_name.satellite_wired_name);
					satellite_num++;
					break;
				case WIRED:
					strcpy(typechar, t_name.wired_name);
					break;
				case OPENVPN_TAP:
					strcpy(typechar, "vpn");
					break;
				default:
					strcpy(typechar, "Unknown");
					break;
			}

			update_devices_info(u);
			print_file_info();

			fprintf(fp, "-----device:%d-----\n", device_num++);
			fprintf(fp, "%s\n", inet_ntoa(u->ip));
			fprintf(fp, "%s\n", ether_etoa(u->mac, mac));
			fprintf(fp, "%s\n", typechar[0] == '\0' ? "Unknown" : typechar);
			fprintf(fp, "%d\n", u->atype);
			fprintf(fp, "%d\n", u->dtype);
			fprintf(fp, "%s\n", u->model[0] == '\0' ? "Unknown" : u->model);
			fprintf(fp, "%s\n", u->host[0] == '\0' ? "Unknown" : host_stod(u->host));
#ifdef SHOW_PARENT_ROUTER
			fprintf(fp, "%s\n", u->parent[0] == '\0' ? "Unknown" : u->parent);
#endif

#ifdef SUPPORT_CIRCLE
			if(circle_writen == 1){
				if(u->dtype != 0){
			fprintf(cfp, "<device>\r\n<IP>%s</IP>\r\n<Name>%s</Name>\r\n<MAC>%s</MAC>\r\n<ConnectionType>%s</ConnectionType>\r\n<DeviceType>%d</DeviceType>\r\n<DeviceModel>%s</DeviceModel>\r\n</device>\r\n", inet_ntoa(u->ip),u->host[0] == '\0' ? "Unknown" : host_stod(u->host),ether_etoa(u->mac, mac),typechar,u->dtype,u->model[0] == '\0' ? "Unknown" : u->model);
				}
			}
			
#endif

#ifdef SUPPORT_STREAMBOOST
			ether_etoa(u->mac, mac);
			if (sb_fp){
				for(sb_dlp = sb_device_list->next; sb_dlp != NULL && strcmp(sb_dlp ->mac, mac) != 0; sb_dlp = sb_dlp->next);
				if(sb_priority_2_netgear_prioriry(u->priority, netgear_priority) == -1){
					/*
					 * Sometimes if no traffic is sent to wan, the default priority of device
					 * set to 255. Once traffic send to wan, the default priority of device
					 * set to 10/20/30/40. We hide this situation for end user and show Low
					 * in GUI for now.
					 */
					strcpy(netgear_priority, "LOW");
				}
				if(sb_dlp){
					/* This attached device was edited by user*/
					/* IP, MAC, Icon type, priority, upload speed, download speed, Device name */
					fprintf(sb_fp, "%s %s %d %s %.2f %.2f %s @#$&*!\n",
					inet_ntoa(u->ip),  mac, sb_dlp->type == TYPE_SOAP_OLD ? u->type : sb_dlp->type, netgear_priority,
					u->state == SB_INFO_STATE_2? u->down : 0.0,
					u->state == SB_INFO_STATE_2? u->up : 0.0,
					sb_dlp->host[0] == '\0' ? (u->host[0] == '\0' ? "&lt;unknown&gt;" : host_stod(u->host)) : host_stod(sb_dlp->host));
				}
				else{
					/* This attached device was NOT edited by user*/
					fprintf(sb_fp, "%s %s %d %s %.2f %.2f %s @#$&*!\n",
					inet_ntoa(u->ip), mac, u->type, netgear_priority,
					u->state == SB_INFO_STATE_2? u->down : 0.0,
					u->state == SB_INFO_STATE_2? u->up : 0.0,
					u->host[0] == '\0' ? "&lt;unknown&gt;" : host_stod(u->host));
				}
			}
			if (name_type_fp){
				if(u->host[0] != '\0')
					fprintf(name_type_fp, "%s %d 0 %s\n", mac, u->type, u->host);
				else
					fprintf(name_type_fp, "%s %d 1 Unknown\n", mac, u->type);
			}
#endif

			if(*pprev == u)
				u = u->next;
			else {
				pprev = &u->next;
				u = u->next;
			}
		}
	}
#ifdef SUPPORT_CIRCLE
	if(circle_writen == 1){
	fprintf(cfp, "</NewAttachDevice>\r\n");
	config_set("save_circle_info","0");
	}
	if(cfp)
	{
		fclose(cfp);
		cfp = NULL;
	}
#endif

#ifdef OPENVPN_TUN_DEVICE
//#define OPENVPN_TUN_IP "192.168"
	if (openvpn_tun_fp) {
		char vpntun_buf[100] = {0};
		int virtual_flag = 0;
		struct arp_struct *tun;
		while (fgets(vpntun_buf, sizeof(vpntun_buf), openvpn_tun_fp)) {
			if (strcasestr(vpntun_buf, "Virtual Address")) {
				virtual_flag = 1;
				continue;
			}
			if (strcasestr(vpntun_buf, "GLOBAL STATS")) {
				virtual_flag = 0;
				continue;
			}
			if (virtual_flag == 0)
				continue;
//			if(strstr(vpntun_buf, OPENVPN_TUN_IP)) {
			char *virtual_addr = NULL, *real_addr = NULL, *common_name = NULL;
			char real_addr_mac[40] = {0};
			virtual_addr = strtok(vpntun_buf, ",");
			common_name = strtok(NULL, ",");
			real_addr = strtok(NULL, ",");
			if (!vpntun_get_arpmac(real_addr, real_addr_mac))
				sprintf(real_addr_mac, "00:00:00:00:00:00");
			strupr(real_addr_mac);
			tun = malloc(sizeof(struct arp_struct));
			if (tun != 0) {
				memset(tun, 0, sizeof(struct arp_struct));
				memcpy(tun->mac, ether_aton(real_addr_mac), ETH_ALEN);
				tun->dtype = DEFAULT_DEVICE_TYPE;
				get_model_name(tun->mac, tun->model);
				update_devices_info(tun);
			}
			fprintf(fp, "-----device:%d-----\n", device_num++);
			fprintf(fp, "%s\n", virtual_addr);
			fprintf(fp, "%s\n", real_addr_mac);
			fprintf(fp, "%s\n", "vpn");
			fprintf(fp, "%d\n", BASE_ATT);
			fprintf(fp, "%d\n", tun->dtype);
			fprintf(fp, "%s\n", tun->model[0] == '\0' ? "Unknown" : tun->model);
			fprintf(fp, "%s\n", "OPENVPN-TUN");
#ifdef SHOW_PARENT_ROUTER
			fprintf(fp, "%s\n", "Orbi Router");
#endif
//			}
			memset(vpntun_buf, 0, sizeof(vpntun_buf));
		}
		fclose(openvpn_tun_fp);
	}
#endif

	if (fp)
		fclose(fp);
	if (hyt_fp)
		fclose(hyt_fp);

	if((fp = fopen(SATELLITE_NUM_FILE, "w"))) {
		fprintf(fp, "%d", satellite_num);
		fclose(fp);
	}

	if((fp = fopen(ATTACHED_DEVICE_NUM_FILE, "w"))) {
		fprintf(fp, "%d", --device_num - satellite_num);
		fclose(fp);
	}
	system("cp /tmp/netscan/attach_device_new /tmp/netscan/attach_device;\
			cp /tmp/netscan/satellite_num_new /tmp/netscan/satellite_num;\
			cp /tmp/netscan/attached_device_num_new /tmp/netscan/attached_device_num");

#ifdef SUPPORT_STREAMBOOST
	if (sb_fp)
		fclose(sb_fp);
	if(name_type_fp)
		fclose(name_type_fp);
	free_device_list(sb_device_list);
#endif
}

/* To fix bug 22146, add function reset_arp_table, it can set active status of all nodes in the arp_tbl to 0 */
void reset_arp_table()
{
	int i;
	struct arp_struct *u;

	total_dev = 0;

	for (i = 0; i < (NEIGH_HASHMASK + 1); i++) {
		for (u = arp_tbl[i]; u; u = u->next) {
			u->active = 0;
			total_dev++;
#ifdef SUPPORT_STREAMBOOST
			u->state = 0;
#endif
		}
	}
#ifdef SUPPORT_STREAMBOOST
	get_streamboost_nodes_info(SB_INFO_STATE_1);
#endif
}

struct ether_arp *fill_arp_packet(const unsigned char *src_mac_addr, const char *src_ip_addr, const unsigned char *dst_mac_addr, const char *dst_ip_addr)
{
	struct ether_arp *arp_packet;
	struct in_addr src_in_addr, dst_in_addr;

	inet_pton(AF_INET, src_ip_addr, &src_in_addr);
	inet_pton(AF_INET, dst_ip_addr, &dst_in_addr);

	arp_packet = (struct ether_arp *)malloc(ETHER_ARP_LEN);
	arp_packet->arp_hrd = htons(ARPHRD_ETHER);
	arp_packet->arp_pro = htons(ETHERTYPE_IP);
	arp_packet->arp_hln = ETH_ALEN;
	arp_packet->arp_pln = IP_ADDR_LEN;
	arp_packet->arp_op = htons(ARPOP_REQUEST);
	memcpy(arp_packet->arp_sha, src_mac_addr, ETH_ALEN);
	memcpy(arp_packet->arp_tha, dst_mac_addr, ETH_ALEN);
	memcpy(arp_packet->arp_spa, &src_in_addr, IP_ADDR_LEN);
	memcpy(arp_packet->arp_tpa, &dst_in_addr, IP_ADDR_LEN);
	return arp_packet;
}

void arp_request(const char *dst_ip_addr, const unsigned char *dst_mac_addr)
{
	struct sockaddr_ll saddr_ll;
	struct ether_header *eth_header;
	struct ether_arp *arp_packet;
	struct ifreq ifr;
	char buf[ETHER_ARP_PACKET_LEN];
	unsigned char src_mac_addr[ETH_ALEN];
	char *src_ip_addr;
	int sock_raw_fd, ret_len;

	if ((sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) == -1)
		goto err;

	bzero(&saddr_ll, sizeof(struct sockaddr_ll));
	bzero(&ifr, sizeof(struct ifreq));
	memcpy(ifr.ifr_name, ARP_IFNAME, strlen(ARP_IFNAME));

	if (ioctl(sock_raw_fd, SIOCGIFINDEX, &ifr) == -1)
		goto err;	
	saddr_ll.sll_ifindex = ifr.ifr_ifindex;
	saddr_ll.sll_family = PF_PACKET;

	if (ioctl(sock_raw_fd, SIOCGIFADDR, &ifr) == -1)
		goto err;	
	src_ip_addr = inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr);

	if (ioctl(sock_raw_fd, SIOCGIFHWADDR, &ifr))
		goto err;	
	memcpy(src_mac_addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

	bzero(buf, ETHER_ARP_PACKET_LEN);
	eth_header = (struct ether_header *)buf;
	memcpy(eth_header->ether_shost, src_mac_addr, ETH_ALEN);
	memcpy(eth_header->ether_dhost, dst_mac_addr, ETH_ALEN);
	eth_header->ether_type = htons(ETHERTYPE_ARP);
	arp_packet = fill_arp_packet(src_mac_addr, src_ip_addr, dst_mac_addr, dst_ip_addr);
	memcpy(buf + ETHER_HEADER_LEN, arp_packet, ETHER_ARP_LEN);

	ret_len = sendto(sock_raw_fd, buf, ETHER_ARP_PACKET_LEN, 0, (struct sockaddr *)&saddr_ll, sizeof(struct sockaddr_ll));
	if ( ret_len < 0)
		DEBUGP("sendto() error!\n");

	close(sock_raw_fd);
	if(arp_packet)
		free(arp_packet);
err: 
	if(sock_raw_fd >= 0)
		close(sock_raw_fd);
	return;
}

void scan_arp_table(int force_soap_flag)
{
	int i;
	int count = 0;
	struct arp_struct *u;
	char *ipaddr, ipaddr_t[32];
	char buffer[512], bmac[32] = {0};
	struct in_addr addr;
	FILE *fp;
	pid_t pid;
	unsigned char br_addr[ETH_ALEN] = BROADCAST_ADDR;

	if(force_soap_flag) {
	        config_set("soap_setting", "AttachDevice");
	        system("/usr/bin/killall -SIGUSR1 soap_agent");
	} else {
		system("/etc/send_soap &");
	}

	pid = fork();
	if(pid < 0)
		DEBUGP("error in fork!\n");
	else if (pid == 0) {
		while (count != 2) {
			count++;
			for (i = 0; i < (NEIGH_HASHMASK + 1); i++) {
				for (u = arp_tbl[i]; u; u = u->next) {
#ifdef HAVE_IGNORE_TABLE
					if (ignore_mac(u->mac))
						continue;
#endif
					if (inet_ntoa(u->ip) != NULL) {
						strcpy(ipaddr_t, inet_ntoa(u->ip));
						arp_request(ipaddr_t, u->mac);
						ether_etoa(u->mac, bmac);
						DEBUGP("Send ARP packet to ip:%s mac:%s\n", ipaddr_t, bmac);
					}
				}
			}
		/**
		 * For beta issue: TD-23
		 * If use Ixia with some virtual DHCP clients to test "Attached Device" feature,
		 * Ixia could not send arp packet actively, we need request all IPs that DHCP server
		 * assigned while user refresh "Attached Device" table.
		 * We just request all IPs in "/tmp/dhcpd_hostlist" that were not recorded in 'arp_tbl'.
		 */

			if ((fp = fopen(DHCP_LIST_FILE,"r"))) {
				while (fgets(buffer, sizeof(buffer), fp)) {
					ipaddr = strtok(buffer, " \t\n");
					if (ipaddr && inet_aton(ipaddr, &addr) != 0) {
#ifdef HAVE_IGNORE_TABLE
						if (ignore_ip(addr))
							continue;
#endif
						for (i = 0; i < (NEIGH_HASHMASK + 1); i++) {
							for (u = arp_tbl[i]; u && memcmp(&u->ip, &addr, sizeof(addr)); u = u->next);
							if (u) break;
						}
						if (u) continue;
						arp_request(ipaddr, br_addr);
						ether_etoa(br_addr, bmac);
						DEBUGP("Send ARP packet to ip:%s mac:%s\n", ipaddr, bmac);
					}
				}
				fclose(fp);
			}
			if(count < 2)
				usleep(500000);
		}
		_exit(0);
	}

	sleep(1);	// sleep one more second to wait soap response
}
