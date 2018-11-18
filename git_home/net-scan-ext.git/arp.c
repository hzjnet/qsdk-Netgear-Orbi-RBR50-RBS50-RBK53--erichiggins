#include "netscan.h"

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
		print_file("/tmp/brctl_showmacs_br0");
		system("iwconfig >> /var/log/netscan/net_scan.log");
		DEBUGP("\n");
		system("(echo \"td s2\";sleep 1) |hyt >> /var/log/netscan/net_scan.log");
		DEBUGP("\n");
	}
}

struct arp_struct
{
	struct arp_struct *next;

	uint16 active;

	struct in_addr ip;
	uint8 mac[ETH_ALEN];
	DeviceType ctype;
	char model[MAX_MODEL_LEN + 1];
	char host[MAX_HOSTNAME_LEN + 1];
};

struct arp_struct *arp_tbl[NEIGH_HASHMASK + 1];

struct arp_struct *arp_tbl[NEIGH_HASHMASK + 1];

static struct arpmsg arpreq;

int init_arp_request(char *ifname)
{
	int s, i;
	struct ifreq ifr;
	struct arpmsg *arp;

	s = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if (s < 0)
		return 0;

	arp = &arpreq;
	memset(arp, 0, sizeof(struct arpmsg));
	memset(&ifr, 0, sizeof(struct ifreq));
	ifr.ifr_addr.sa_family = PF_PACKET;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

	/*judge whether ARP_IFNAME has an IP,if not sleep 5s*/
	for (i=5; i >= 0; i--){
		if (i == 0)
			return 0;
		if (ioctl(s, SIOCGIFADDR, &ifr) != 0)
			sleep(5);
		else
			break;
	}
	memcpy(arp->ar_sip, &((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr, 4);

	if (ioctl(s, SIOCGIFHWADDR, &ifr) != 0)
		return 0;
	memset(arp->h_dest, 0xFF, 6);
	memcpy(arp->h_source, ifr.ifr_hwaddr.sa_data, 6);
	arp->h_proto = htons(ETH_P_ARP);
	arp->ar_hrd = htons(ARPHRD_ETHER);
	arp->ar_pro = htons(ETH_P_IP);
	arp->ar_hln = 6;
	arp->ar_pln = 4;
	arp->ar_op = htons(ARPOP_REQUEST);
	memcpy(arp->ar_sha, ifr.ifr_hwaddr.sa_data, 6);

	close(s);
	return 1;
}

void acl_update_name(uint8 *mac, char *name)
{
	char dev_mac[32];

	dni_system(NULL, "/usr/sbin/acl_update_name", ether_etoa(mac, dev_mac), name, NULL);
}

static int total_dev = 0;

int update_arp_table(uint8 *mac, struct in_addr ip, int isrepl)
{
	uint32 i;
	char host[MAX_HOSTNAME_LEN + 1] = {0}, bmac[32] = {0};
	struct arp_struct *u;

	/* check dhcp host */
	i = arp_hash(mac);
	for (u = arp_tbl[i]; u && memcmp(u->mac, mac, ETH_ALEN); u = u->next);
	if (u) {
		if (*host) {
			strncpy(u->host, host, MAX_HOSTNAME_LEN);
			acl_update_name(u->mac, host);
		}
		u->ip = ip;              /* The IP may be changed for DHCP      */
		u->active = 1;
		return isrepl;	/* Do BIOS Name Query only ARP reply */
	}

	ether_etoa(mac, bmac);
	DEBUGP("Receive a ARP packet: HASH_VALUE:%d IP:%s MAC:%s\n", i, inet_ntoa(ip), bmac);
	u = malloc(sizeof(struct arp_struct));
	if (u == 0)
		return 0;
	memset(u, 0, sizeof(struct arp_struct));
	u->ip = ip;
	u->active = 1;
	memcpy(u->mac, mac, ETH_ALEN);
	total_dev++;

	if (*host) {
		strncpy(u->host, host, MAX_HOSTNAME_LEN);
		acl_update_name(u->mac, host);
	}


	u->next = arp_tbl[i];
	arp_tbl[i] = u;

	printf("There is a new device coming, IP:%s MAC:%s\n", inet_ntoa(u->ip), bmac);
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
		return -1;

	/* We're trying to override buffer size  to set a bigger buffer. */
	if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize)))
		DEBUGP("setsocketopt error!\n");

	me->sa_family = PF_PACKET;
	strncpy(me->sa_data, ARP_IFNAME, 14);
	if (bind(s, me, sizeof(*me)) < 0)
		return -1;
	if (init_arp_request(ARP_IFNAME) == 0)
		return -1;

	return s;
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

struct typechar_name t_name;

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
	if(p == NULL)
		return -1;
	strcpy(t_name.wired_name, p);
	p = strtok(NULL, "-");
	if(p == NULL)
		return -1;
	strcpy(t_name.main_2g_name, p);
	p = strtok(NULL, "-");
	if(p == NULL)
		return -1;
	strcpy(t_name.main_5g_name, p);
//#ifdef HAVE_GUEST_NETWORK 
	p = strtok(NULL, "-");
	if(p == NULL)
		return -1;
	strcpy(t_name.guest_2g_name, p);
	p = strtok(NULL, "-");
	if(p == NULL)
		return -1;
	strcpy(t_name.guest_5g_name, p);
//#endif
#ifdef HAVE_GUEST_WIRELESS2
	p = strtok(NULL, "-");
	if(p == NULL)
		return -1;
	strcpy(t_name.byod_2g_name, p);
	p = strtok(NULL, "-");
	if(p == NULL)
		return -1;
	strcpy(t_name.byod_5g_name, p);
#endif

	fclose(tfp);
	return 0;

}

void show_arp_table(void)
{
	int i, device_num = 1;
	unsigned int mac_num = 0;
	char mac[32], bmac[32] = {0};
	struct arp_struct *u;
	struct arp_struct **pprev;
	FILE *fp = NULL, *brfd = NULL;

	get_typechar_name();
	get_interface_info();
	if((brfd = fopen("/tmp/brctl_showmacs_br0", "r")) == NULL)
		DEBUGP("cannot open /tmp/brctl_showmacs_br0\n");

	fp = fopen(ARP_FILE, "w");
	if (fp == NULL) {
		DEBUGP("[net-scan]open %s failed\n", ARP_FILE);
		return;
	}

	for (i = 0; i < (NEIGH_HASHMASK + 1); i++) {
		for (pprev = &arp_tbl[i], u = *pprev; u; ) {
			char typechar[32] = {0};

			mac_num++;
			u->ctype = get_device_type(u->mac, brfd);
			if (u->active == 0 || u->ctype == NONETYPE) {
				DEBUGP("No Active, don't display: [%s] [%s]\n", ether_etoa(u->mac, mac), inet_ntoa(u->ip));
				if (mac_num > 255) {
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

			/* for GUI dealing easily:  &lt;unknown&gt;   <----> <unknown>*/
#ifdef HAVE_GUEST_WIRELESS2
			if (u->ctype == WIRELESS_2G_WIRELESS2 ||u->ctype == WIRELESS_5G_WIRELESS2 ||u->ctype == WIRELESS_2G ||u->ctype == WIRELESS_2G_GUEST ||u->ctype == WIRELESS_5G ||u->ctype == WIRELESS_5G_GUEST ||u->ctype == WIRED) {
#else
			if (u->ctype == WIRELESS_2G ||u->ctype == WIRELESS_2G_GUEST ||u->ctype == WIRELESS_5G ||u->ctype == WIRELESS_5G_GUEST ||u->ctype == WIRED) {
#endif
				switch (u->ctype) {
					case WIRELESS_2G:
							strcpy(typechar, t_name.main_2g_name);
						break;
					case WIRELESS_2G_GUEST:
							strcpy(typechar, t_name.guest_2g_name);
						break;
					case WIRELESS_5G:
							strcpy(typechar, t_name.main_5g_name);
						break;
					case WIRELESS_5G_GUEST:
							strcpy(typechar, t_name.guest_5g_name);
						break;
#ifdef HAVE_GUEST_WIRELESS2
					case WIRELESS_2G_WIRELESS2:
							strcpy(typechar, t_name.byod_2g_name);
						break;
					case WIRELESS_5G_WIRELESS2:
							strcpy(typechar, t_name.byod_5g_name);
						break;
#endif
					case WIRED:
							strcpy(typechar, t_name.wired_name);
						break;
				}
				DEBUGP("-----device:%d-----\n", device_num);
				DEBUGP("%s\n", inet_ntoa(u->ip));
				DEBUGP("%s\n", ether_etoa(u->mac, mac));
				DEBUGP("%d\n", u->ctype);
				DEBUGP("0\n");	//attched type
				DEBUGP("0\n"); //device type
				DEBUGP("%s\n", u->model[0] == '\0' ? "Unknown" : u->model);
				DEBUGP("%s\n", u->host[0] == '\0' ? "Unknown" : host_stod(u->host));

				fprintf(fp, "-----device:%d-----\n", device_num++);
				fprintf(fp, "%s\n", inet_ntoa(u->ip));
				fprintf(fp, "%s\n", ether_etoa(u->mac, mac));
				fprintf(fp, "%s\n", typechar);
				fprintf(fp, "0\n");	//attached type
				fprintf(fp, "0\n"); //device type 
				fprintf(fp, "%s\n", u->model[0] == '\0' ? "Unknown" : u->model);
				fprintf(fp, "%s\n", u->host[0] == '\0' ? "Unknown" : host_stod(u->host));
				fprintf(fp, "%s,%s\n", "ParentAP","ParentAP");//Consistent with the file format in Base
			}

			if (*pprev == u)
				u = u->next;
			else {
				pprev = &u->next;
				u = u->next;
			}
		}
	}
	print_file_info();

	fclose(fp);
	if(brfd) 
		fclose(brfd);
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
		}
	}
}

void scan_arp_table(int sock, struct sockaddr *me)
{
	int i;
	int count = 1, sum = 0;
	struct arpmsg *req;
	struct arp_struct *u;
	char *ipaddr;
	char buffer[512], bmac[32] = {0};
	struct in_addr addr;
	FILE *fp;

	pid_t pid;

	pid = fork();
	if (pid < 0)
		DEBUGP("[%s][%d]error in fork!\n", __FILE__, __LINE__);
	else if(pid == 0) {
	while (count != 3) {
		count++;
		req = &arpreq;
		for (i = 0; i < (NEIGH_HASHMASK + 1); i++) {
			for (u = arp_tbl[i]; u; u = u->next) {
				memcpy(req->ar_tip, &u->ip, 4);
				memcpy(req->ar_tha, &u->mac, 6);
				memcpy(req->h_dest, &u->mac, 6);
				sendto(sock, req, sizeof(struct arpmsg), 0, me, sizeof(struct sockaddr));
				ether_etoa(u->mac, bmac);
				DEBUGP("Send ARP packet to ip:%s mac:%s\n", inet_ntoa(u->ip), bmac);
				sum++;
				if (sum == 64) {
					usleep(500000);
					sum = 0;
				}
			}
		}
	}
	_exit(0);
	}
}
