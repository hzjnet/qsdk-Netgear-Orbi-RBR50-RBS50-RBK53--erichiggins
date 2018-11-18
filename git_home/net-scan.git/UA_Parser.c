#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>
#include <errno.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <ctype.h>

#include <netinet/tcp.h>
#include <netinet/ip.h>
#include "netscan.h"

#define TIMERINTERVAL	60
#define CHECKTIME		15
#define PIDFILE	"/var/run/UA_Parser.pid"
//#define LOGFILE	"/tmp/UA.log"
#define NETLINK_XHTTP 22
#define BUFLEN 4096
#define UH_LIMIT_HEADERS	64
#define UH_HTTP_MSG_GET		0
#define UH_HTTP_MSG_HEAD	1
#define UH_HTTP_MSG_POST	2
#define UH_HTTP_MSG_PUT		3
#define UH_HTTP_MSG_DELETE	4

#define array_size(x) \
	(sizeof(x) / sizeof(x[0]))

#define foreach_header(i, h) \
	for( i = 0; (i + 1) < (sizeof(h) / sizeof(h[0])) && h[i]; i += 2 )

typedef struct nl_packet_msg {
	size_t data_len;
	size_t http_len;
	unsigned char payload[0];
} nl_packet_msg_t;

struct http_request {
	int	method;
	float version;
	int redirect_status;
	char *url;
	char *headers[UH_LIMIT_HEADERS];
	struct auth_realm *realm;
};

char *strfind(char *haystack, int hslen, const char *needle, int ndlen)
{
	int match = 0;
	int i, j;

	for( i = 0; i < hslen; i++ )
	{
		if( haystack[i] == needle[0] )
		{
			match = ((ndlen == 1) || ((i + ndlen) <= hslen));

			for( j = 1; (j < ndlen) && ((i + j) < hslen); j++ )
			{
				if( haystack[i+j] != needle[j] )
				{
					match = 0;
					break;
				}
			}

			if( match )
				return &haystack[i];
		}
	}

	return NULL;
}

static struct http_request * uh_http_header_parse(char *buffer, int buflen)
{

	char *headers = NULL;
	char *hdrname = NULL;
	char *hdrdata = NULL;

	int i;
	int hdrcount = 0;

	static struct http_request req;

	memset(&req, 0, sizeof(req));

	/* terminate initial header line */
	if( (headers = strfind(buffer, buflen, "\r\n", 2)) != NULL )
	{
		*headers++ = 0;
		*headers++ = 0;

		/* process header fields */
		for( i = (int)(headers - buffer); i < buflen; i++ )
		{
			/* found eol and have name + value, push out header tuple */
			if( hdrname && hdrdata && (buffer[i] == '\r' || buffer[i] == '\n') )
			{
				buffer[i] = 0;
				/* store */
				if( (hdrcount + 1) < array_size(req.headers) )
				{
					req.headers[hdrcount++] = hdrname;
					req.headers[hdrcount++] = hdrdata;

					hdrname = hdrdata = NULL;
				}

				/* too large */
				else
					return NULL;
			}

			/* have name but no value and found a colon, start of value */
			else if( hdrname && !hdrdata &&
			    ((i+1) < buflen) && (buffer[i] == ':')
			) {
				buffer[i] = 0;
				hdrdata = &buffer[i+1];

				while ((hdrdata + 1) < (buffer + buflen) && *hdrdata == ' ')
					hdrdata++;
			}

			/* have no name and found [A-Za-z], start of name */
			else if( !hdrname && isalpha(buffer[i]) )
			{
				hdrname = &buffer[i];
			}
		}

		/* valid enough */
		req.redirect_status = 200;
		return &req;
	}

	/* Malformed request */
	return NULL;
}
/*
 * ----device:num----
 device ip
 device mac
 connection type
 attached type
 device type
 device model
 device host
 device parent
 **/

void get_mac_and_host_by_ip(char *mac, char *host, char *ip) {
	FILE *fp;
	int  row = 1;
	char line[64], item[32], *newline;
	system("killall -SIGUSR1 net-scan; sleep 1");
	fp = fopen("/tmp/netscan/attach_device", "r");
	if (fp == NULL) return;

	while (fgets(line, sizeof(line), fp) != NULL) {
		sprintf(item, "----device:%d----", row);
		if (strstr(line, item) == NULL)
			continue;
		fgets(line, sizeof(line), fp);
		newline = strtok(line, "\n");
		if (strcmp(newline, ip) == 0) {
			fgets(line, sizeof(line), fp);
			newline = strtok(line, "\n");
			strncpy(mac, newline, 32);
			fgets(line, sizeof(line), fp);
			fgets(line, sizeof(line), fp);
			fgets(line, sizeof(line), fp);
			fgets(line, sizeof(line), fp);
			fgets(line, sizeof(line), fp);
			newline = strtok(line, "\n");
			strncpy(host, newline, 255);
			break;
		}
		row++;
	}

	fclose(fp);
	return;
}

struct os os_table[] = {
	{"Windows", "Windows PC", "38"},
	{"Longhorm", "Windows PC", "38"},
	{"Android","Android Devices", "2"},
	{"Unix","Unix PC", "37"},
	{"Linux","Linux PC", "19"},
	{"Mac OS","Mac PC", "21"},
	{NULL,NULL}
};

void get_model_name_from_UA(char *ua, char *ip) {
	int scount, i, j, find_f = 0, wild_f;
	char *p;
	char mac[32], host[255], model[32], type[32], cmd[512], *sku = NULL, *type_t = NULL;
	cJSON *dataArray, *Item, *skuItem, *modelItem, *typeItem;

	memset(mac, 0x00, 32);
	memset(model, 0x00, 32);
	memset(type, 0x00, 32);
	memset(host, 0x00, 255);
	memset(cmd, 0x00, 512);
	
	if((dataArray = getJSONcode(UA_FILE)) != NULL) {
		scount = cJSON_GetArraySize(dataArray);

		for(i = 0; i < scount; i++) {
			Item  = cJSON_GetArrayItem(dataArray, i);
			if (Item == NULL)
				break;

			skuItem = cJSON_GetObjectItem(Item, "Device SKU");
			modelItem = cJSON_GetObjectItem(Item, "Device Model");
			typeItem = cJSON_GetObjectItem(Item, "Device Type");
			sku = get_wild_character(skuItem->valuestring);
			find_f = wild_f = j = 0;
			if(sku == NULL) {
				wild_f = 1;
				sku = skuItem->valuestring;
			}

			p = strstr(ua, sku);
			if (p) {
				DEBUGP("find model:%s sku:%s type:%s ip:%s\n", modelItem->valuestring, skuItem->valuestring, typeItem->valuestring, ip);
				if(wild_f && invalid_wild(p, sku))
					break;
				find_f = 1;
				strcpy(model, modelItem->valuestring);
				strtok(typeItem->valuestring, "\(");
				type_t = strtok(NULL, ")");
				if (type_t != NULL)
					strcpy(type, type_t);
				break;
			}
		}
	}
	cJSON_Delete(dataArray);

	if(!find_f)
		for (; os_table[j].os_kernel != NULL; j++)
			if (strstr(ua, os_table[j].os_kernel)) {
				strcpy(model, os_table[j].os_model);
				strcpy(type, os_table[j].os_type);
				break;
			}
	get_mac_and_host_by_ip(mac, host, ip);
	mac[sizeof(mac)-1] = '\0';
	type[sizeof(type)-1] = '\0';
	model[sizeof(model)-1] = '\0';
	host[sizeof(host)-1] = '\0';
	if ((mac[0] != '\0') && (host[0] != '\0') && (type[0] != '\0') && (model[0] != '\0')) {
		snprintf(cmd, sizeof(cmd), "devices_info update \"%s\" \"%s\" \"%s\" \"%s\"", mac, type, model, host);
		cmd[sizeof(cmd)-1] = '\0';
		system(cmd);
	}
	return;
}

void get_ip_string(unsigned int hexip, char *ip) {
	char *org;
	org = &hexip;
	snprintf(ip, 32, "%u.%u.%u.%u", org[3], org[2], org[1], org[0]);
}

void XHTTP_rule(int flag) {
	if (flag == 0) {
		system("iptables -t mangle -D PREROUTING -i br0 -p tcp --tcp-flags ALL PSH,ACK --dport 80 -j XHTTP");
		system("iptables -t mangle -D PREROUTING -i br0 -p tcp --tcp-flags ALL PSH,ACK --dport 8080 -j XHTTP");
	} else if (flag == 1) {
		system("iptables -t mangle -I PREROUTING 1 -i br0 -p tcp --tcp-flags ALL PSH,ACK --dport 80 -j XHTTP");
		system("iptables -t mangle -I PREROUTING 1 -i br0 -p tcp --tcp-flags ALL PSH,ACK --dport 8080 -j XHTTP");
	} else {
		system("[ \"x$(iptables -t mangle -nvL |grep XHTTP)\" = \"x\" ] && iptables -t mangle -I PREROUTING 1 -i br0 -p tcp --tcp-flags ALL PSH,ACK --dport 80 -j XHTTP && iptables -t mangle -I PREROUTING 1 -i br0 -p tcp --tcp-flags ALL PSH,ACK --dport 8080 -j XHTTP");
	}
}

int check_count;

void signal_handler(int sig) {
	if (sig == SIGALRM) {
		check_count++;
		if (check_count != CHECKTIME) {
			XHTTP_rule(2);
			//reset timer
			struct itimerval tv;
			tv.it_value.tv_sec = TIMERINTERVAL;
			tv.it_value.tv_usec = 0;
			tv.it_interval.tv_sec = 0;
			tv.it_interval.tv_usec = 0;
			setitimer(ITIMER_REAL, &tv, 0);
		}
	}

	if (check_count == CHECKTIME || sig == SIGTERM || sig == SIGINT || sig == SIGQUIT) {
		XHTTP_rule(0);
		if (sig == SIGALRM)
			DEBUGP("time out\n");
		unlink(PIDFILE);
		exit(0);
	}
}

int main(int argc, char ** argv)
{
	int sock_fd,  pid;
	struct sockaddr_nl loc_addr, peer_addr;
	static unsigned char *buf;
	struct nlmsghdr *nlh;
	unsigned int addrlen;
//	struct stat fstat;
//	char old_file[132] = {0};
	struct sigaction sa;
	struct sigaction sa_term;
	struct sigaction sa_int;
	struct sigaction sa_quit;
	struct itimerval tv;
	FILE *fp;
	char s[32];

//	if (stat(LOGFILE, &fstat) == 0 && fstat.st_size > 5242880) {
//		sprintf(old_file, "%s.old", LOGFILE);
//		rename(LOGFILE, old_file);
//	}
//	fd = open(LOGFILE, O_CREAT | O_APPEND | O_RDWR);
//	if (fd == -1) {
//		fprintf(stderr, "[%s][%d]open error:%d, %s\n", __FILE__, __LINE__, errno, strerror(errno));
//		return -1;
//	}
//	dup2(fd, 1);

	daemon(1, 1);

	pid = getpid();
	if (access(PIDFILE, F_OK) == 0) {
		fp = fopen(PIDFILE, "r");
		if(fp == NULL) return -1;
		fgets(s, sizeof(s), fp);
		if (atoi(s) != pid){
			fclose(fp);
			DEBUGP("UA_Parser already open\n");
			return -1;
		}
		fclose(fp);
	} else {
		fp = fopen(PIDFILE, "w");
		if(fp == NULL) return -1;
		fprintf(fp, "%d", pid);
		fclose(fp);
	}

	system("[ \"x$(lsmod | grep 'ipt_xhttp' >/dev/null 2>&1)\" != \"x0\" ] && rmmod /lib/modules/$(uname -r)/ipt_xhttp.ko >/dev/null");
	system("[ \"x$(lsmod | grep 'ipt_xhttp' >/dev/null 2>&1)\" = \"x\" ] && insmod /lib/modules/$(uname -r)/ipt_xhttp.ko >/dev/null");

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = signal_handler;
	memset(&sa_term, 0, sizeof(sa_term));
	sa_term.sa_flags = SA_RESTART;
	sa_term.sa_handler = signal_handler;
	memset(&sa_int, 0, sizeof(sa_int));
	sa_int.sa_flags = SA_RESTART;
	sa_int.sa_handler = signal_handler;
	memset(&sa_quit, 0, sizeof(sa_quit));
	sa_quit.sa_flags = SA_RESTART;
	sa_quit.sa_handler = signal_handler;

	sigaction(SIGALRM, &sa, NULL);
	sigaction(SIGTERM, &sa_term, NULL);
	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);

	check_count=0;
	tv.it_value.tv_sec = TIMERINTERVAL;
	tv.it_value.tv_usec = 0;
	tv.it_interval.tv_sec = 0;
	tv.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &tv, 0);

	sock_fd = socket(PF_NETLINK, SOCK_DGRAM, 7);
	memset(&loc_addr, 0, sizeof(struct sockaddr_nl));
	loc_addr.nl_family = AF_NETLINK;
	loc_addr.nl_pid = getpid();
	loc_addr.nl_groups = 1;
	if ( bind(sock_fd, (struct sockaddr*)&loc_addr, sizeof(loc_addr)) == -1 ) {
		DEBUGP("bind error:%d, %s\n", errno, strerror(errno));
		goto err;
	}

	memset(&peer_addr, 0, sizeof(struct sockaddr_nl));
	peer_addr.nl_family = AF_NETLINK;
	peer_addr.nl_pid = 0;
	peer_addr.nl_groups = 1;

	addrlen = sizeof(peer_addr);
	buf = (unsigned char *)malloc(BUFLEN);

	XHTTP_rule(1);
	while(1) {
		nl_packet_msg_t *upkt;
		struct iphdr *iph;
		struct http_request *req;
		int i = 0;
		char sip[32], dip[32];
		memset(buf, 0, BUFLEN);

		ssize_t retlen = recvfrom(sock_fd, buf, BUFLEN, 0, (struct sockaddr*)&peer_addr, &addrlen);
		*(buf+(BUFLEN-1)) = '\0';
		if ( retlen <= 0 || addrlen != sizeof(peer_addr) || peer_addr.nl_pid != 0 ) {
			DEBUGP("recvfrom error, retlen:%d, %d, %s\n",  retlen, errno, strerror(errno));
			continue;
		}

		nlh = (struct nlmsghdr *)buf;

		if (nlh->nlmsg_len > retlen) {
			DEBUGP("nlmsg error, %d, %s\n", errno, strerror(errno));
			continue;
		}

		upkt = NLMSG_DATA((struct nlmsghdr *)(buf));
		iph = (struct iphdr *) upkt->payload;

		get_ip_string(ntohl(iph->saddr), sip);
		get_ip_string(ntohl(iph->daddr), dip);
		DEBUGP("src addr:%s, dest addr:%s, len:%d, ip protocol:%d, data_len:%d, http_len:%d\n", sip, dip, ntohs(iph->tot_len),iph->protocol, upkt->data_len, upkt->http_len);

		req = uh_http_header_parse((&((char *)upkt->payload)[upkt->data_len - upkt->http_len]), (int)(upkt->http_len));

		if (req) {
			DEBUGP("Get request\n");
			foreach_header(i, req->headers) {
				if( ! strncasecmp(req->headers[i], "User-Agent", UH_LIMIT_HEADERS) ) {
					DEBUGP("Get User-Agent:%s\n", req->headers[i+1]);
					get_model_name_from_UA(req->headers[i+1], sip);
					break;
				}
			}
		}
	}

err:
	close(sock_fd);
	unlink(PIDFILE);
	return 1;
}
