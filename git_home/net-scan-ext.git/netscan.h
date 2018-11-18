#ifndef _NET_SCAN_H
#define _NET_SCAN_H

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include <sys/stat.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <syslog.h>

#define ARP_IFNAME	"br0"
#define ARP_FILE	"/tmp/netscan/attach_device"
#define ARP_FILE_WIRED	"/tmp/netscan/attach_device_wired"
#define ARP_FILE_2G	"/tmp/netscan/attach_device_2g"
#define ARP_FILE_5G	"/tmp/netscan/attach_device_5g"
#define DHCP_LIST_FILE	"/tmp/dhcpd_hostlist"
#define WLAN_STA_FILE	"/tmp/sta_dev"
#define MAC_OUI_FILE	"/tmp/device_tables/oui.csv"
#define MAC_MAM_FILE	"/tmp/device_tables/mam.csv"
#define MAC_OUI36_FILE	"/tmp/device_tables/oui36.csv"
#define LOCK_FILE	"/var/run/netscan.pid"

/* lock mode of lockfile */
#define LOCKMODE 	(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

/* The max length of NETBIOS name is 15, the max length of DHCP hostanme is 255 (BOOTP/DHCP option 12) */
#define MAX_HOSTNAME_LEN	255
#define MAX_MODEL_LEN	25

//#define DEBUG_SWITCH 1
//#ifdef DEBUG_SWITCH
#define DEBUGP(x, args...) print_log(__FILE__, __LINE__, x, ## args)
//#else
//#define DEBUGP(fmt,args...) /* do nothing*/
//#endif

#define NEIGH_HASHMASK	0x1F

typedef enum interface_type {
	NONETYPE_IF,
	WIRELESS_2G_IF,
	WIRELESS_2G_GUEST_IF,
	WIRELESS_5G_IF,
	WIRELESS_5G_GUEST_IF,
#ifdef HAVE_GUEST_WIRELESS2
	WIRELESS_2G_WIRELESS2_IF,
	WIRELESS_5G_WIRELESS2_IF,
#endif
	WIRED_IF,
	BACKHAUL_IF
}InterfaceType_t;

typedef enum device_type{
	NONETYPE,
	WIRELESS_2G,
	WIRELESS_2G_GUEST,
 	WIRELESS_5G,
	WIRELESS_5G_GUEST,
#ifdef HAVE_GUEST_WIRELESS2
	WIRELESS_2G_WIRELESS2,
	WIRELESS_5G_WIRELESS2,
#endif
	WIRED,
	BACKHAUL
}DeviceType;


typedef unsigned int	uint32;
typedef unsigned short	uint16;
typedef unsigned char	uint8;
int debug_log;

struct arpmsg
{
	/* Ethernet header */
	uint8	h_dest[6];	/* destination ether addr */
	uint8	h_source[6];	/* source ether addr */
	uint16	h_proto;		/* packet type ID field */

	/* ARP packet */
	uint16	ar_hrd;	/* hardware type (must be ARPHRD_ETHER) */
	uint16	ar_pro;	/* protocol type (must be ETH_P_IP) */
	uint8	ar_hln;	/* hardware address length (must be 6) */
	uint8	ar_pln;	/* protocol address length (must be 4) */
	uint16	ar_op;	/* ARP opcode */
	uint8	ar_sha[6];	/* sender's hardware address */
	uint8	ar_sip[4];	/* sender's IP address */
	uint8	ar_tha[6];	/* target's hardware address */
	uint8	ar_tip[4];	/* target's IP address */

	uint8	pad[18];	/* pad for min. Ethernet payload (60 bytes) */
} __attribute__ ((packed, aligned(4)));

struct nb_request
{
	uint16	xid;
 	uint16	flags;
	uint16	questions;
	uint16	answer;
 	uint16	authority;
	uint16	additional;

	char		qname[34];

	uint16	qtype;
	uint16	qclass;
} __attribute__ ((packed));

struct nb_response_header
{
	uint16	xid;
	uint16	flags;
	uint16	questions;
	uint16	answer;
	uint16	authority;
	uint16	additional;

	char		qname[34];

	uint16	qtype;
	uint16	qclass;

	uint32	ttl;
	uint16	datalen;
	uint8	num_names;
} __attribute__ ((packed));

struct typechar_name
{
	char wired_name[32];
	char main_2g_name[32];
	char main_5g_name[32];
//#ifdef HAVE_GUEST_NETWORK
	char guest_2g_name[32];
	char guest_5g_name[32];
//#endif 
#ifdef HAVE_GUEST_WIRELESS2
	char byod_2g_name[32];
	char byod_5g_name[32];
#endif
};

extern int open_arp_socket(struct sockaddr *me);
extern int open_bios_socket(void);
extern int dni_system(const char *output, const char *cmd, ...);

extern int recv_arp_pack(struct arpmsg *arpkt, struct in_addr *send_ip);
extern void recv_bios_pack(char *buf, int len, struct in_addr from);

extern void update_bios_name(uint8 *mac, char *host, struct in_addr ip);
extern void send_bios_query(int sock, struct in_addr dst_ip);

extern void reset_arp_table();
extern void scan_arp_table(int sock, struct sockaddr *me);
extern void show_arp_table(void);

extern char *config_get(char *name);
extern void update_satellite_name();
extern void update_satellite_name_arp_table(uint8 *mac, struct in_addr ip, char *host);

extern uint32 arp_hash(uint8 *pkey);
extern void get_dhcp_host(char host[], struct in_addr ip, int *isrepl);
extern void remove_disconn_dhcp(struct in_addr ip);
extern char *ether_etoa(uint8 *e, char *a);
extern void strupr(char *str);
extern int check_sta_format(char *info);
extern char *host_stod(char *s);
extern char *get_mac(char *ifname, char *eabuf);
extern int get_device_type(uint8 *mac, FILE *fd);
//extern int get_interface_info();
extern void get_model_name(uint8 *mac, char *model);

#endif
