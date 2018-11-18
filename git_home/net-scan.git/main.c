/*
 * NETGEAR SPEC:
 * 8.8 Attached Devices
 * We only show IP address, device name and MAC address on the attached devices list.
 *** How to build the attached device table?
 * The attached device table should contain IP address, device name and MAC address.
 * 1. Listen all the ARP packets (including request and reply, not from router itself which means
 * don't record sender MAC/IP address of router) on the LAN and record all sender MAC
 * address, sender IP address and target IP address in the attached ARP device table.
 * 2. Once we receive any reply packet, we should send out name query to that client to get device
 * name.
 * Note: The NetBIOS is only supported by Microsoft Windows system or Linux with Samba. That's
 * why we use this method to build initial table.
 *** How to validate the attached device table?
 * When customer clicks the "Refresh" button, the following steps should be implemented.
 * 1. Send ARP packets to all IP addresses belong to our attach device table. Wait 3 seconds to
 * collect all replies. If any client doesn't reply, then remove it from our attach device table.
 * 2. Please follow "how to build the attached device table" to add any new device in the network
 * to our attach device table.
 * 3. Update the attach device table to the GUI.
 *
 * refer to arping.c - Ping hosts by ARP requests/replies
 *         BY
 * Author:	Alexey Kuznetsov <kuznet@ms2.inr.ac.ru>
 * Busybox port: Nick Fedchik <nick@fedchik.org.ua>
 *
 * net-scan is not so good, but at least can work. :) --- 20071217
 */

#include "netscan.h"

static int sigval;
time_t refresh_time, auto_time;
static int scan_flag = 1;

int lockfile(int fd)
{
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	return(fcntl(fd, F_SETLK, &fl));
}

int already_running(void)
{
	int		fd;
	char	buf[16];
	char	*lock_file;

	lock_file = LOCK_FILE;
	fd = open(lock_file, O_RDWR|O_CREAT, LOCKMODE);
	if (fd < 0) {
		syslog(LOG_ERR, "can't open %s: %s", lock_file, strerror(errno));
		exit(1);
	}
	if (lockfile(fd) < 0) {
		if (errno == EACCES || errno == EAGAIN) {
			close(fd);
			return(1);
		}
		syslog(LOG_ERR, "can't lock %s: %s", lock_file, strerror(errno));
		exit(1);
	}
	ftruncate(fd, 0);
	sprintf(buf, "%ld", (long)getpid());
	write(fd, buf, strlen(buf)+1);
	return(0);
}

void signal_pending(int sig)
{
	sigval = sig;
	if (sigval == SIGALRM) {
		struct itimerval tv;
		tv.it_value.tv_sec = 900;
		tv.it_value.tv_usec = 0;
		tv.it_interval.tv_sec = 0;
		tv.it_interval.tv_usec = 0;
		setitimer(ITIMER_REAL, &tv, 0);
	}
	if (sigval == SIGTERM) {
		printf("Receive SIGTERM, net-scan process will exit normally!!!\n");
		exit(0);
	}
}

void handle_sigchld(void)
{
	pid_t pid;
	int status, num = 0;

	while((++num < 100) && (pid = waitpid(-1, &status, WNOHANG)) > 0)
		DEBUGP("The child %d exit with code %d\n", pid, WEXITSTATUS(status));
	return;
}

void do_signal()
{
	int forse_soap = 0;
	time_t now;
	if (sigval == 0)
		return;

	if (sigval == SIGUSR2 || sigval == SIGALRM) {
		debug_log = config_match("netscan_debug", "1");
#ifdef HAVE_IGNORE_TABLE
		update_ignore_table();
#endif

		/* when GUI click refresh, update arp table first, then send arp packets */
		if (sigval == SIGUSR2) {
			/* start catch HTTP packets when GUI refresh */
			if(strcmp(config_get("support_trend_micro_qos"),"1") != 0)
				system("/usr/bin/killall UA_Parser; sleep 1; /usr/sbin/UA_Parser");
			forse_soap = 1;
		} else
			DEBUGP("15 minutes passed or devices leave\n");

		now = time(NULL);
		if (now > refresh_time && now - refresh_time < 10) {
			DEBUGP("refresh too quickly, not to send arp packets, last time:%d, now:%d\n", refresh_time, now);
			sigval = 0;
			return;
		}
		refresh_time = now;
		scan_flag = 0;

		/* To fix bug 22146, call reset_arp_table to set active status of all nodes in the arp_tbl to 0 in the parent process */
		reset_arp_table();
		scan_arp_table(forse_soap);
		sleep(2);
		handle_sigchld();
		scan_flag = 1;
	} else if (sigval == SIGUSR1 && scan_flag) {
		now = time(NULL);
		if (config_invmatch("save_circle_info", "1")) {
			if (now > auto_time && now - auto_time < 15)
				DEBUGP("Not to send SOAP query, last time:%d, now:%d\n", auto_time, now);
			else {
				system("/etc/send_soap &");
				auto_time = now;
			}
		}
		show_arp_table();
	}
	sigval = 0;
}

int main(int argc, char **argv)
{
	int arp_sock;
	int bios_sock;
	int max_sock;

	fd_set readset;
	struct sigaction sa_usr1;
	struct sigaction sa_usr2;
	struct sigaction sa_alrm;
	struct sigaction sa_term;
	struct sockaddr me;
	struct itimerval tv;
	
	if(init_interface_info() == -1){
		fprintf(stderr,"Fail to open interface config file!!!\n");
		exit(1);
	}

	arp_sock = open_arp_socket(&me);
	bios_sock = open_bios_socket();

	if (arp_sock < 0 ||bios_sock < 0)
		exit(1);

	if (access(LOG_DIR, F_OK) != 0 && mkdir(LOG_DIR, 0755))
		fprintf(stderr, "can't create %s\n", LOG_DIR);

	printf("The attached devices demo is Running ...\n");
	daemon(1, 1);

	/*
	* Make sure only one copy of the daemon is running.
	*/
	if (already_running()) {
		syslog(LOG_ERR, "daemon already running");
		exit(1);
	}

	refresh_time=0;

	memset(&sa_usr1, 0, sizeof(sa_usr1));
	sa_usr1.sa_flags = SA_RESTART;
	sa_usr1.sa_handler = signal_pending;
	memset(&sa_usr2, 0, sizeof(sa_usr2));
	sa_usr2.sa_flags = SA_RESTART;
	sa_usr2.sa_handler = signal_pending;
	memset(&sa_alrm, 0, sizeof(sa_alrm));
	sa_alrm.sa_flags = SA_RESTART;
	sa_alrm.sa_handler = signal_pending;
	memset(&sa_term, 0, sizeof(sa_term));
	sa_term.sa_flags = SA_RESTART;
	sa_term.sa_handler = signal_pending;

	sigaction(SIGUSR1, &sa_usr1, NULL);
	sigaction(SIGUSR2, &sa_usr2, NULL);
	sigaction(SIGALRM, &sa_alrm, NULL);
	sigaction(SIGTERM, &sa_term, NULL);
	signal(SIGCHLD, SIG_IGN);

	if (bios_sock > arp_sock)
		max_sock = bios_sock + 1;
	else
		max_sock = arp_sock + 1;

	tv.it_value.tv_sec = 900;
	tv.it_value.tv_usec = 0;
	tv.it_interval.tv_sec = 0;
	tv.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &tv, 0);

	while (1) {
		int ret;
		int alen = 0;
		int blen = 0;

		struct in_addr send_ip;
		struct sockaddr arp_from;
		struct sockaddr_in bios_from;

		socklen_t arp_len = sizeof(arp_from);
		socklen_t bios_len = sizeof(bios_from);

		struct arpmsg	arp_packet;
		char bios_packet[1024];

		FD_ZERO(&readset);
		FD_SET(arp_sock, &readset);
		FD_SET(bios_sock, &readset);

		ret = select(max_sock, &readset, 0, 0, 0);
		if (ret <= 0) {
			if (ret == -1 && errno == EINTR)
				do_signal();
			continue;
		}

		if (FD_ISSET(arp_sock, &readset))
			alen = recvfrom(arp_sock, &arp_packet, sizeof(arp_packet), 0, 
				(struct sockaddr *) &arp_from, &arp_len);
		if (FD_ISSET(bios_sock, &readset))
			blen = recvfrom(bios_sock, bios_packet, sizeof(bios_packet), 0, 
				(struct sockaddr *) &bios_from, &bios_len);
		bios_packet[sizeof(bios_packet)-1] = '\0';
		/* min arp packet length: 14 ethernet + 28 arp header = 42 */
		if (alen >= 42 && recv_arp_pack(&arp_packet, &send_ip))
			send_bios_query(bios_sock, send_ip);
		if (blen > 0)
			recv_bios_pack(bios_packet, blen, bios_from.sin_addr);

		do_signal();
	}

	return 0;
}


