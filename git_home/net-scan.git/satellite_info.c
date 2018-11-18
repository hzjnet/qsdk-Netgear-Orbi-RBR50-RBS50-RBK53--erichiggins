#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

#include "netscan.h"

typedef struct type_length_value {
	int type;
	int len;
	char val[0];
} s_tlv;

char *dev_info_type[] = {
	"NONE",
	"mac address",
	"ip address",
	"module name",
	"device name",
	"current version",
	"serial number",
	"MAX_TYPE"		/* no use */
};

#define MAX_FILES 100
#define MAX_FILE_NAME 32
#define HYT_FILE "/tmp/hyt_result"
#define SOAPCLIENT_DEVINFO_FILE "/tmp/soapclient/dev_info/"


char files[MAX_FILES][MAX_FILE_NAME];

static int get_all_satellite_mac(void)
{
	bzero(files, sizeof(files));

	FILE *fp = fopen(HYT_FILE, "r");
	if (fp == NULL) {
		DEBUGP("can't open file /tmp/hyt_result\n");
		return -1;
	}

	int idx = 0;
	char buf[128];
	while (fgets(buf, sizeof(buf), fp)) {
		strtok(buf, ",");
		strncpy(files[idx++], buf, sizeof(buf));
		DEBUGP("find a connected satellite: %s\n", buf);
	}

	fclose(fp);
	return 0;
}

static int parse_file(char *file, char **name)
{
	DEBUGP("parse contents of %s\n", file);

	char buf[4096] = {0};
	FILE *fp = fopen(file, "r");
	if (!fp) {
		DEBUGP("can't open file %s !\n", file);
		return -1;
	} else {
		fread(buf, sizeof(buf), 1, fp);
		fclose(fp);
	}

	char *p = buf;
	char mac[18], ip[16], dev_name[128];

	bzero(mac, sizeof(mac));
	bzero(ip, sizeof(ip));
	bzero(dev_name, sizeof(dev_name));
	
	while (1) {
		s_tlv *tlv = (s_tlv *)p;
		if (tlv->type == 0 || p >= (buf + sizeof(buf)) || name[tlv->type] == NULL)
			break;

		/* spec define max length of release note is 512 bytes, 1024 is enough */

		if(!strcmp(name[tlv->type], "mac address"))
			strncpy(mac, tlv->val, tlv->len);
		if(!strcmp(name[tlv->type], "ip address"))	
			strncpy(ip, tlv->val, tlv->len);
		if(!strcmp(name[tlv->type], "device name")){	
			struct in_addr ip_addr;
			struct ether_addr *mac_addr;
			strncpy(dev_name, tlv->val, tlv->len);
			DEBUGP("Get satellite info mac:%s ip:%s device name:%s\n", mac, ip, dev_name);
			ip_addr.s_addr = inet_addr(ip);
			mac_addr = ether_aton(mac);
			update_satellite_name_arp_table(&mac_addr->ether_addr_octet, ip_addr, dev_name); 
			bzero(mac, sizeof(mac));
			bzero(ip, sizeof(ip));
			bzero(dev_name, sizeof(dev_name));
		}
		p += offsetof(s_tlv, val) + tlv->len;
	}
}

/* each file include one record */
void update_satellite_name()
{
	if (get_all_satellite_mac())
		return;

	char file[128];
	int idx;
	char **name = dev_info_type;

	for (idx = 0; idx < MAX_FILES; idx++)
		if (files[idx][0] != '\0') {
			bzero(file, sizeof(file));
			sprintf(file, "%s%s", SOAPCLIENT_DEVINFO_FILE, files[idx]);
			parse_file(file, name);
		}
}
