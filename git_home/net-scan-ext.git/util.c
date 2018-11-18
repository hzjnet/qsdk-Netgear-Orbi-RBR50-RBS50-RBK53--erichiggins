#include "netscan.h"

#define INTERFACENUM_MAX 12

struct interface_element{
	char name[32];
	char mac[18];
	int port;
	InterfaceType_t if_type;
};

/* modified from "linux-2.4.18/net/ipv4/arp.c" */
uint32 arp_hash(uint8 *pkey)
{
#define GET_UINT32(p)	((p[0]) |(p[1] << 8) |(p[2] << 16) |(p[3] << 24))
	uint32 hash_val;
	if(pkey == NULL)
		return 0;

	hash_val = GET_UINT32(pkey);
	hash_val ^= hash_val >> 16;
	hash_val ^= hash_val >> 8;
	hash_val ^= hash_val >> 3;

	return hash_val & NEIGH_HASHMASK;
}

void get_dhcp_host(char host[], struct in_addr ip, int *isrepl)
{
	FILE *tfp;
	char *ipaddr;
	char *hostname;
	char *ipstr;
	char buff[512];

	host[0] = '\0';
	ipstr = inet_ntoa(ip);
	if ((tfp = fopen(DHCP_LIST_FILE,"r")) == 0)
		return;

	while (fgets(buff, sizeof(buff), tfp)) {
		ipaddr = strtok(buff, " \t\n");
		hostname = strtok(NULL, "\t\n");
		if (ipaddr == NULL || hostname == NULL)
			continue;

		if (strcmp(ipaddr, ipstr) == 0) {
			strncpy(host, hostname, MAX_HOSTNAME_LEN);
			*isrepl = 0;
			break;
		}
	}

	fclose(tfp);
}

//void remove_disconn_dhcp(struct in_addr ip)
//{
//	int i, k, result;
//	int target = 0;
//	int target_num = 0;
//	FILE *fp;
//	fpos_t pos_w,pos_r,pos;
//	char ipaddr[32];
//	char line[512];
//	char list_str[512];
//
//	if ( !(fp = fopen (DHCP_LIST_FILE,"r")))
//		return;
//	
//	while(fgets(line, sizeof(line), fp) != NULL) {
//		result = sscanf(line, "%s%s", ipaddr,list_str);
//		if (result == 2){
//			if(memcmp(inet_ntoa(ip), ipaddr, strlen(ipaddr)) == 0) {
//				target = 1;
//				break;
//			}
//		}
//		target_num ++;
//	}
//	fclose(fp);
//
//	if (target != 1)
//		return;
//
//	if ( !(fp = fopen (DHCP_LIST_FILE,"r+")))
//		return;
//	for (i = 0; i < target_num; i++)
//		fgets(line,sizeof(line),fp);
//	
//	/* save the file pointer position */
//	fgetpos (fp,&pos_w);
//	/* position the delete line */
//	fgets(line,sizeof(line),fp);
//	fgetpos (fp,&pos_r);
//	pos = pos_r;
//
//	while (1)
//	{
//		/* set a new file position */ 
//		fsetpos (fp,&pos);
//		if (fgets(line,sizeof(line),fp) ==NULL) 
//			break;
//		fgetpos (fp,&pos_r);
//		pos = pos_w;
//		fsetpos (fp,&pos);
//		fprintf(fp,"%s",line);
//		fgetpos (fp,&pos_w);
//		pos = pos_r;
//	}
//	pos = pos_w;
//	fsetpos (fp,&pos);
//	k = strlen(line);
//	for (i=0;i<k;i++) 
//		fputc(0x20,fp);
//	
//	fclose(fp);
//}

char *ether_etoa(uint8 *e, char *a)
{
	static char hexbuf[] = "0123456789ABCDEF";

	int i, k;

	for (k = 0, i = 0; i < 6; i++) {
		a[k++] = hexbuf[(e[i] >> 4) & 0xF];
		a[k++] = hexbuf[(e[i]) & 0xF];
		a[k++]=':';
	}

	a[--k] = 0;

	return a;
}

/*
 * xss Protection 
 * < -> &lt;
 * > -> &gt;
 * ( -> &#40;
 * ) -> &#41;
 * " -> &#34;
 * ' -> &#39;
 * # -> &#35;
 * & -> &#38;
 */
char *host_stod(char *s)
{//change special character to ordinary string
	static char str[MAX_HOSTNAME_LEN*5 + 1 ];
	char c, *p;

	p = str;
        while((c = *s++) != '\0') {
                if(c == '"'){
                        *p++ = '&'; *p++ = '#'; *p++ = '3'; *p++ = '4'; *p++ = ';';
                } else if( c == '(' ){
                        *p++ = '&'; *p++ = '#'; *p++ = '4'; *p++ = '0'; *p++ = ';';
                } else if( c == ')' ){
                        *p++ = '&'; *p++ = '#'; *p++ = '4'; *p++ = '1'; *p++ = ';';
                } else if( c == '#' ){
                        *p++ = '&'; *p++ = '#'; *p++ = '3'; *p++ = '5'; *p++ = ';';
                } else if( c == '&' ){
                        *p++ = '&'; *p++ = '#'; *p++ = '3'; *p++ = '8'; *p++ = ';';
                } else if( c == '<' ){
                        *p++ = '&'; *p++ = 'l'; *p++ = 't'; *p++ = ';';
                } else if( c == '>' ){
                        *p++ = '&'; *p++ = 'g'; *p++ = 't'; *p++ = ';';
                } else if (c == '\'') {
                        *p++ = '&'; *p++ = '#'; *p++ = '3'; *p++ = '9'; *p++ = ';';
                }
                else {
                        *p++ = c;
                }
        }
        *p = '\0';

	return str;
}

void strupr(char *str)
{
	for(;*str != '\0'; str++)
	{
		if(*str >= 97 && *str <= 122)
			*str = (*str)-32;
	}
}

int check_sta_format(char *info)
{
	int i,len;

	len = strlen(info);

	if(len < 26 || len > 30)
		return 0;

	for(i=0; i<len; i++)
	{
		if((i==2 || i==5 || i==8 || i==11 || i==14) && *(info+i)!= ':')
			return 0;

		if(*(info+17) > 32) // ignore those are not space or Tab
			return 0;
	}

	return 1;
}

char *get_mac(char *ifname, char *eabuf)
{
	int s;
	struct ifreq ifr;

	eabuf[0] = '\0';
	s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (s < 0)
		return eabuf;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0)
		ether_etoa((uint8 *)ifr.ifr_hwaddr.sa_data, eabuf);
	close(s);

	return eabuf;
}

InterfaceType_t get_ifstr(char *str)
{
	if(!strcmp(str,"wired"))return WIRED_IF;
	else if(!strcmp(str,"2G"))return WIRELESS_2G_IF;
	else if(!strcmp(str,"5G"))return WIRELESS_5G_IF;
 	else if(!strcmp(str,"2G-GUEST"))return WIRELESS_2G_GUEST_IF;
 	else if(!strcmp(str,"5G-GUEST"))return WIRELESS_5G_GUEST_IF;
#ifdef HAVE_GUEST_WIRELESS2
	else if(!strcmp(str,"2G-WIRELESS2"))return WIRELESS_2G_WIRELESS2_IF;
	else if(!strcmp(str,"5G-WIRELESS2"))return WIRELESS_5G_WIRELESS2_IF;
#endif
	else if(!strcmp(str,"backhaul_if"))return BACKHAUL_IF;
	else return NONETYPE_IF;
}
struct interface_element if_ele[INTERFACENUM_MAX];
int interfacenum_max;
int init_interface_info()
{
	FILE *fp;
	int i;
	char buff[128],*ifname,*ctype,*port;
	if((fp = fopen("/tmp/netscan_if_conf","r")) == NULL)
	{
		fprintf(stderr,"[Net-Scan]Error open if config file!!!\n");
		return -1;
	}
	// Initial Inteface Configure
	for (i = 0; i < INTERFACENUM_MAX; i++){
		strncpy(if_ele[i].name, "", sizeof(if_ele[i].name));
		strncpy(if_ele[i].mac, "00:00:00:00:00:00", sizeof(if_ele[i].mac));
		if_ele[i].port = 0;
		if_ele[i].if_type = NONETYPE_IF;
	}
	//Get interface info from file
	while(fgets(buff, sizeof(buff), fp)){
		ifname = strtok(buff, " ");
		ctype = strtok(NULL, " ");
		port = strtok(NULL, " ");
		if(ifname == NULL || ctype == NULL || port == NULL)
				continue;
		for (i = 0; i < INTERFACENUM_MAX; i++){
			if(strcmp(if_ele[i].name, "") == 0) {
				strcpy(if_ele[i].name,ifname);
				if_ele[i].if_type = get_ifstr(ctype);
				if_ele[i].port = atoi(port);
				break;
			}
		}
	}
	return 0;
}


int get_interface_info () {
	FILE *fd;
	int i;
	char buff[512], *port, *bmac, *other;
	char iface_name[32];
	int repacd_mode;
	
	system("brctl showmacs br0 > /tmp/brctl_showmacs_br0");
	if((fd = fopen("/tmp/brctl_showmacs_br0", "r")) == NULL)
		return -1;

	//get mac addresses to compare
	for (i = 0; i < INTERFACENUM_MAX; i++){
		if(strcmp(if_ele[i].name, "") == 0) break;
		get_mac(if_ele[i].name, if_ele[i].mac);
	}

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
		other = strtok(NULL, " \t\n");
		if(port == NULL || bmac == NULL)
			continue;

		strupr(bmac);
		for (i = 0; i < INTERFACENUM_MAX; i++){
			if(strcmp(if_ele[i].name, "") == 0) break;
			if(!strncmp(bmac, if_ele[i].mac, 17)){
				if_ele[i].port = atoi(port);
				DEBUGP("Found %s interface port id %d\n", if_ele[i].name, if_ele[i].port);
				break;
			}
		}
	}

	//print interface infomation
	DEBUGP("---Interface Table---\n");
	for (i = 0; i < INTERFACENUM_MAX; i++){
		if(strcmp(if_ele[i].name, "") == 0) break;
		DEBUGP("name:%s MAC:%s port:%d\n", if_ele[i].name, if_ele[i].mac, if_ele[i].port);
	}

	fclose(fd);
	return 0;
}

int get_device_type(uint8 *mac, FILE *fd) {
	FILE *ethfd = NULL;
	int found = NONETYPE, i, repacd_mode = 0, eth_mode = 0;
	char buff[512], arpmac[32], *port, *bmac, *other;


	repacd_mode = atoi(config_get("repacd_Daisy_Chain_Enable"));
	if((ethfd = fopen("/tmp/link_status_eth", "r")) != NULL) {
		fgets(buff, sizeof(buff), ethfd);
		eth_mode = atoi(strtok(buff, "\t\n"));
		fclose(ethfd);
	}

	if(fd == NULL) return NONETYPE;
	fseek(fd, 0, 0); //reset fd pointer to brctl_showmacsbr0 file start

	/*
	 * #brctl showmacs br0
	 * port no		mac addr			is local?		ageing time
	 * 1			20:14:07:11:2A:20	yes				1.01
	 * ......
	 */

	fgets(buff, sizeof(buff), fd);		//skip first line
	ether_etoa(mac, arpmac);
	while(fgets(buff, sizeof(buff), fd)){
		int portnum=0;

		port = strtok(buff, " \t\n");
		bmac = strtok(NULL, " \t\n");
		other = strtok(NULL, " \t\n");
		if(port == NULL || bmac == NULL)
			continue;

		strupr(bmac);
		if(strncmp(arpmac, bmac, strlen(arpmac)))
			continue;

		portnum = atoi(port);
		DEBUGP("ONE client[mac:%s] from %d interface.\n", bmac, portnum);
		for (i = 0; i < INTERFACENUM_MAX; i++)
		{
			// For Orbi projects 2.4G is ath0, ath02. 5G is ath1,ath11
			if(strcmp(if_ele[i].name, "") == 0) break;
			if (if_ele[i].port == portnum && repacd_mode) {
				switch(if_ele[i].if_type) {
					case WIRED_IF:
						if (!eth_mode)
							found = WIRED;
						else
							found = BACKHAUL;//ignore lan port when satellite is in the ethernet backhaul mode
						goto ret;
					case WIRELESS_2G_IF:
						found = WIRELESS_2G;
						goto ret;
					case WIRELESS_2G_GUEST_IF:
						if(config_match("wlg1_endis_guestNet"), "1")
							found = WIRELESS_2G_GUEST;
#ifdef HAVE_GUEST_WIRELESS2
						else
							found = WIRELESS_2G_WIRELESS2;
#endif
						goto ret;
					case WIRELESS_5G_IF:
						found = WIRELESS_5G;
						goto ret;
					case WIRELESS_5G_GUEST_IF:
						if(config_match("wlg1_endis_guestNet"), "1")
							found = WIRELESS_5G_GUEST;
#ifdef HAVE_GUEST_WIRELESS2
						else
							found = WIRELESS_5G_WIRELESS2;
#endif
						goto ret;
#ifdef HAVE_GUEST_WIRELESS2
					case WIRELESS_2G_WIRELESS2_IF:
						found = WIRELESS_2G_WIRELESS2;
						goto ret;
					case WIRELESS_5G_WIRELESS2_IF:
						found = WIRELESS_5G_WIRELESS2;
						goto ret;
#endif
					case BACKHAUL_IF:
						found = BACKHAUL;
						goto ret;
				}
			}
			else if (if_ele[i].port == portnum){
				switch(if_ele[i].if_type) {
					case WIRED_IF:
						if (!eth_mode)
							found = WIRED;
						else
							found = BACKHAUL;//ignore lan port when satellite is in the ethernet backhaul mode
						goto ret;
					case WIRELESS_2G_IF:
						found = WIRELESS_2G;
						goto ret;
					case WIRELESS_2G_GUEST_IF:
						if(config_match("wlg1_endis_guestNet"), "1")
							found = WIRELESS_2G_GUEST;
#ifdef HAVE_GUEST_WIRELESS2
						else
							found = WIRELESS_2G_WIRELESS2;
#endif
						goto ret;
					case WIRELESS_5G_IF:
						found = WIRELESS_5G;
						goto ret;
					case WIRELESS_5G_GUEST_IF:
						if(config_match("wlg1_endis_guestNet"), "1")
							found = WIRELESS_5G_GUEST;
#ifdef HAVE_GUEST_WIRELESS2
						else
							found = WIRELESS_5G_WIRELESS2;
#endif
						goto ret;
#ifdef HAVE_GUEST_WIRELESS2
					case WIRELESS_2G_WIRELESS2_IF:
						found = WIRELESS_2G_WIRELESS2;
						goto ret;
					case WIRELESS_5G_WIRELESS2_IF:
						found = WIRELESS_5G_WIRELESS2;
						goto ret;
#endif
					case BACKHAUL_IF:
						found = BACKHAUL;
						goto ret;
				}
			}
		}
	}

ret:
	return found;
}

int rmsemicolon(char *mac, char *newmac) {
	int i, j;
	for (i = 0, j = 0; i < 32 && mac[i] != '\0'; i++)
		if ((mac[i] >= '0' && mac[i] <= '9') || (mac[i] >= 'A' && mac[i] <= 'F'))
			newmac[j++] = mac[i];
		else if (mac[i] == ':')
			continue;
		else
			return -1;
	newmac[j]='\0';
	return 0;
}

void get_vendor(char *mac, char *file, int len, char *model) {
	FILE *fp;
	char line[256], *newline, *oui, *name, newmac[32];

	fp = fopen(file, "r");
	if (fp == NULL) {
		DEBUGP("Cannot open file %s\n", file);
		return NULL;
	}

	if (rmsemicolon(mac, newmac) < 0) return;

	while (fgets(line, sizeof(line), fp)) {
		newline = strtok(line, "\n");
		oui = strtok(newline, ",");
		name = strtok(NULL, ",");
		if (strncmp(newmac, oui, len) == 0) {
			strncpy(model, name, MAX_MODEL_LEN);
			DEBUGP("find vendor name:%s oui:%s mac:%s\n", name, oui, mac);
			break;
		}
	}
	fclose(fp);
	return;
}

void get_model_name_from_oui(char *mac, char *model) {
	char *name = NULL;
	get_vendor(mac, MAC_OUI_FILE, 6, model);
	if (model[0] != '\0') return;
	get_vendor(mac, MAC_MAM_FILE, 7, model);
	if (model[0] != '\0') return;
	get_vendor(mac, MAC_OUI36_FILE, 9, model);
	if (model[0] != '\0') return;
}

void get_model_name(uint8 *macint, char *model) {
	char mac[32], name;

	ether_etoa(macint, mac);

	get_model_name_from_oui(mac, model);
	return;
}

void print_log(char *file, int line, const char *fmt, ...)
{
	if(debug_log) {
		char log_file[128] = {0}, old_file[128] = {0}, buf[128] = {0};
		struct stat fs;
		va_list ap;
		FILE *fp;

		time_t t = time(NULL);
		char tm[64] = {0};
		strftime(tm, sizeof(tm), "%Y-%m-%d %H:%M:%S", localtime(&t));

		if (access("/var/log/netscan", F_OK) != 0)
			system("mkdir /var/log/netscan");
		sprintf(log_file, "%snet_scan.log", "/var/log/netscan/");
		sprintf(old_file, "%s.old", log_file);

		if ((fp = fopen(log_file, "a")) == NULL) {
			fprintf(stderr, "cannot open %s", log_file);
			return;
		}
		int fd = fileno(fp);

		bzero(&fs, sizeof(fs));
		if (fstat(fd, &fs) == 0 && fs.st_size > 102400) {
			fclose(fp);
			rename(log_file, old_file);
			if ((fp = fopen(log_file, "w")) == NULL) {
				fprintf(stderr, "cannot open %s", log_file);
				return;
			}
		}
		
		va_start(ap, fmt);
		vsnprintf(buf, sizeof(buf), fmt, ap);
		va_end(ap);

		fprintf(fp, "{%s}[%s:%d] %s", time, file, line, buf);
		fclose(fp);
	}
}
