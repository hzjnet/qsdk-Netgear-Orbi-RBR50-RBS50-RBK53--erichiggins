#include "netscan.h"

/* modified from "linux-2.4.18/net/ipv4/arp.c" */
uint32 arp_hash(uint8 *pkey)
{
#define GET_UINT32(p)	((p[0]) |(p[1] << 8) |(p[2] << 16) |(p[3] << 24))
	uint32 hash_val;

	if (pkey == NULL)
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

		if (strncmp(ipstr, ipaddr, strlen(ipstr)) == 0) {
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
	char c, *p, *q;

	if ( s == NULL )
		return NULL;

	q = s; p = str;
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
		// remove whitespace at the end of the string
		s--;
		while((s >= q) && ((c = *--s) == ' '))
			*--p = '\0';
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

cJSON *getJSONcode(char *filename) {
	FILE *tfp;
	cJSON *root;
	int flen=0;
	char *json_buf;

	root=NULL;
	if((tfp = fopen(filename, "r")) == NULL)
		return NULL;

	fseek(tfp,0L,SEEK_END);
	flen=ftell(tfp);
	json_buf=(char *)malloc(flen+1);
	fseek(tfp,0L,SEEK_SET);
	fread(json_buf,flen,1,tfp);
	json_buf[flen]='\0';
	fclose(tfp);
	root = cJSON_Parse(json_buf);
	free(json_buf);
	json_buf = NULL;
	return root;
}

char *get_wild_character(char *sku_t) {
	char *p = NULL, *sku = NULL;

	if ( sku_t == NULL )
		return NULL;

	p = sku_t;
	if (*p++ == '*') {
		sku = p;
		p = p + strlen(p);
		if((*(--p) == '*') && (p >= sku))
			*p = '\0';
	}
	return sku;
}

int invalid_wild(char *ua_t, char *sku) {
	int flag = 1;

	if(ua_t == NULL || sku == NULL)
		return flag;

	if((*(--ua_t) == '(') || (*ua_t == ' ')) {
		ua_t = ua_t + 1 + strlen(sku);
		if(*ua_t == ';')
			flag = 0;
	}
	return flag;
}

struct vendor vendor_table[] = {
	{"LCFC Electronics Technology", "Lenovo"},
	{"Hewlett Packard", "Hewlett Packard"},
	{"NETGEAR", "NETGEAR"},
	{"HUAWEI", "HUAWEI"},
	{"Apple", "Apple"},
	{"Microsoft", "Microsoft"},
	{"HTC", "HTC"},
	{"Google", "Google"},
	{"Intel", "Intel"},
	{"BUFFALO", "BUFFALO"},
	{"zte", "zte"},
	{"Brocade", "Brocade"},
	{"TP-LINK", "TP-LINK"},
	{"Cisco", "Cisco"},
	{"Cisco-Linksys", "Cisco-Linksys"},
	{"ASUSTek", "ASUSTek"},
	{"Avaya", "Avaya"},
	{"TRENDnet", "TRENDnet"},
	{"Toshiba", "Toshiba"},
	{"SHENZHEN MERCURY", "SHENZHEN MERCURY"},
	{"NEC", "NEC"},
	{"AzureWave", "AzureWave"},
	{"Vodafone", "Vodafone"},
	{"MITSUMI", "MITSUMI"},
	{"Philips", "Philips"},
	{"LG Innotek", "LG"},
	{"Letv Mobile and Intelligent Information Technology Corporation", "Letv"},
	{"OMRON", "OMRON"},
	{"Sony", "Sony"},
	{"ELECOM", "ELECOM"},
	{"Panasonic", "Panasonic"},
	{"Sonos", "Sonos"},
	{"D-Link", "D-Link"},
	{"Nokia", "Nokia Corporation"},
	{"Gionee", "Gionee"},
	{"Lenovo", "Lenovo"},
	{"SK hynix", "SK hynix"},
	{"Samsung", "Samsung"},
	{"FUJITSU", "FUJITSU"},
	{"Actiontec", "Actiontec"},
	{"Yahoo", "Yahoo"},
	{"AmTRAN", "AmTRAN"},
	{"SHARP", "SHARP"},
	{"Seagate", "Seagate"},
	{"Nortel Networks", "Nortel Networks"},
	{"Motorola", "Motorola"},
	{"EMC Corporation", "EMC Corporation"},
	{"Mitsubishi", "Mitsubishi"},
	{"Lorex Technology", "Lorex Technology"},
	{"Bosch", "Bosch"},
	{"AVerMedia Information", "AVerMedia Information"},
	{"Siemens", "Siemens"},
	{"Acer", "Acer"},
	{"Dell", "Dell"},
	{"ARCHOS", "ARCHOS"},
	{"AT&T", "AT&T"},
	{"BenQ", "BenQ"},
	{"Amazon", "Amazon"},
	{"Xiaomi", "Xiaomi"},
	{"ZyXEL", "ZyXEL"},
	{"ARRIS", "ARRIS"},
	{"Razer", "Razer"},
	{"MSI", "MSI"},
	{"Juniper", "Juniper"},
	{NULL, NULL}
};

int vendor_filter(char *name)
{
	int i = 0;
	for (; vendor_table[i].vendor_s != NULL; i++)
	{
		if (strcasestr(name, vendor_table[i].vendor_s) != 0)
				break;
	}
	return i;
}

void get_vendor(char *mac, char *file, int len, char *model) {
	FILE *fp;
	char line[256], *newline, *oui, *name, newmac[32];
	int i;

	fp = fopen(file, "r");
	if (fp == NULL) {
		DEBUGP("Cannot open file %s\n", file);
		return NULL;
	}

	if (rmsemicolon(mac, newmac) < 0)
	{
		fclose(fp);
		return;
	}
	while (fgets(line, sizeof(line), fp)) {
		newline = strtok(line, "\n");
		oui = strtok(newline, ",");
		name = strtok(NULL, ",");
		if (strncmp(newmac, oui, len) == 0) {
			i = vendor_filter(name);
			if ((i >= 0) && (vendor_table[i].vendor_m != NULL))
				strncpy(name, vendor_table[i].vendor_m, MAX_MODEL_LEN);
			strncpy(model, name, MAX_MODEL_LEN);
			DEBUGP("find vendor name:%s oui:%s mac:%s\n", name, oui, mac);
			break;
		}
	}
	fclose(fp);
	return;
}

void get_model_name_from_oui(char *mac, char *model) {
	get_vendor(mac, MAC_OUI_FILE, 6, model);
	if (model[0] != '\0') return;
	get_vendor(mac, MAC_MAM_FILE, 7, model);
	if (model[0] != '\0') return;
	get_vendor(mac, MAC_OUI36_FILE, 9, model);
	if (model[0] != '\0') return;
}

void get_model_name(uint8 *macint, char *model) {
	char mac[32];

	ether_etoa(macint, mac);

	if (strcmp(model, "Netgear") == 0)
		return;

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
		char time[64] = {0};
		strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", localtime(&t));

		sprintf(log_file, "%snet_scan.log", LOG_DIR);
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
