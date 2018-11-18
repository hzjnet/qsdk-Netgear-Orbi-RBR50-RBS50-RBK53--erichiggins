/*
 **************************************************************************
 * Copyright (c) 2017, Circle Media Inc.  All rights reserved.
 **************************************************************************
 */
#include <linux/module.h>
#include <linux/of.h>

#include <linux/version.h>
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/skbuff.h>
#include <linux/icmp.h>
#include <linux/sysctl.h>
#include <linux/debugfs.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <asm/unaligned.h>
#include <linux/uaccess.h>	/* for put_user */
#include <net/ipv6.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <linux/etherdevice.h>

#include "include/ecm_classifier_pcc_public.h"

#define RULE_FIELDS 5

/*
 * DebugFS entry object.
 */
static struct dentry *ecm_pcc_test_dentry;

/*
 * Registration
 */
struct ecm_classifier_pcc_registrant *ecm_pcc_test_registrant = NULL;

/*
 * Rule table
 */
struct ecm_pcc_test_rule {
	struct list_head list;
	ecm_classifier_pcc_result_t accel;
	unsigned int proto;
	int src_port;
	int dest_port;
	struct in6_addr src_addr;
	struct in6_addr dest_addr;
	unsigned int ipv;
};
LIST_HEAD(ecm_pcc_test_rules);
DEFINE_SPINLOCK(ecm_pcc_test_rules_lock);

/*
 * ecm_pcc_test_registrant_ref()
 *	Invoked when an additional hold is kept upon the registrant
 */
static void ecm_pcc_test_registrant_ref(struct ecm_classifier_pcc_registrant *r)
{
	int remain;

	/*
	 * Increment the ref count by 1.
	 * This causes the registrant structure to remain in existance until
	 * released (deref).
	 * By definition the caller of this method has a hold on the registrant
	 * already so it cannot 'go away'.
	 * This is because either:
	 * 1. The caller itself has been passed it in a function parameter;
	 * 2. It has its own explicit hold.
	 */
	remain = atomic_inc_return(&r->ref_count);
	if (remain <= 0)
		pr_info("REFERENCE COUNT WRAP!\n");
	else
		/*pr_info("ECM PCC Registrant ref: %d\n", remain)*/;
}

/*
 * ecm_pcc_test_registrant_deref()
 *	Caller is releasing its hold upon the registrant.
 */
static void
ecm_pcc_test_registrant_deref(struct ecm_classifier_pcc_registrant *r)
{
	int remain;
	struct ecm_pcc_test_rule *tmp = NULL;
	struct list_head *pos, *q;

	/*
	 * Decrement the reference count
	 */
	remain = atomic_dec_return(&r->ref_count);
	if (remain > 0) {
		/*
		 * Something still holds a reference
		 */
		//pr_info("ECM PCC Registrant deref: %d\n", remain);
		return;
	}

	/*
	 * Last hold upon the registrant is released and so we can now
	 * destroy it.
	 */
	if (remain < 0)
		pr_info("REFERENCE COUNT WRAP!\n");

	spin_lock_bh(&ecm_pcc_test_rules_lock);
	list_for_each_safe(pos, q , &ecm_pcc_test_rules) {
		tmp = list_entry(pos, struct ecm_pcc_test_rule, list);
		list_del(pos);
		kfree(tmp);
	}
	spin_unlock_bh(&ecm_pcc_test_rules_lock);

	pr_info("Circle ECM PCC Registrant DESTROYED\n");
	kfree(r);
}

static ecm_classifier_pcc_result_t
ecm_pcc_test_okay_to_accel_v4(struct ecm_classifier_pcc_registrant *r,
			      uint8_t *src_mac, __be32 src_ip, int src_port,
			      uint8_t *dest_mac, __be32 dest_ip, int dest_port,
			      int protocol)
{
	/*printk(KERN_ERR " %08X:%04d - " " %08X:%04d proto=%d",
			ntohl(src_ip), ntohs((unsigned short)(src_port)),
			ntohl(dest_ip), ntohs((unsigned short)(dest_port)),
			protocol
			);*/
	return ECM_CLASSIFIER_PCC_RESULT_PERMITTED;
}

static ecm_classifier_pcc_result_t
ecm_pcc_test_okay_to_accel_v6(struct ecm_classifier_pcc_registrant *r,
			      uint8_t *src_mac, struct in6_addr *src_addr,
			      int src_port, uint8_t *dest_mac,
			      struct in6_addr *dest_addr, int dest_port,
			      int protocol)
{
	return ECM_CLASSIFIER_PCC_RESULT_PERMITTED;
}


static int ecm_pcc_test_unregister_get_unregister(void *data, u64 *val)
{
	*val = 0;
	return 0;
}


static int ecm_pcc_test_unregister_set_unregister(void *data, u64 val)
{
	if (ecm_pcc_test_registrant) {
		pr_info("Circle ECM PCC unregister\n");
		ecm_classifier_pcc_unregister_begin(ecm_pcc_test_registrant);
		ecm_pcc_test_registrant = NULL;
	}
	return 0;
}

/*
 * ecm_pcc_test_str_to_ip()
 *	Convert string IP to in6_addr.  Return 4 for IPv4 or 6 for IPv6.
	Return something else in error.
 *
 * NOTE: When string is IPv4 the lower 32 bit word of the in6 address
 * contains the address. Network order.
 */
static unsigned int ecm_pcc_test_str_to_ip(char *ip_str, struct in6_addr *addr)
{
	uint8_t *ptr = (uint8_t *)(addr->s6_addr);

	/*
	 * IPv4 address in addr->s6_addr32[0]
	 */
	if (in4_pton(ip_str, -1, ptr, '\0', NULL) > 0)
		return 4;

	/*
	 * IPv6
	 */
	if (in6_pton(ip_str, -1, ptr, '\0', NULL) > 0)
		return 6;

	return 0;
}

/*
 * ecm_pcc_test_deny_accel()
 *	Deny acceleration
 */
static void ecm_pcc_test_deny_accel(unsigned int proto,
		struct in6_addr *src_addr, struct in6_addr *dest_addr,
		int src_port, int dest_port,
		unsigned int ipv)
{
	char mac[ETH_ALEN]={0}; //dummy
	if (ipv == 4)
		ecm_classifier_pcc_deny_accel_v4(mac, src_addr->s6_addr32[0], src_port, 
						mac, dest_addr->s6_addr32[0], dest_port, 
						proto);
	else
		ecm_classifier_pcc_deny_accel_v6(mac, src_addr, src_port,
						mac, dest_addr, dest_port,
						proto);
}

/*
 * ecm_pcc_test_permit_accel()
 *	Permit acceleration
 */
static void ecm_pcc_test_permit_accel(unsigned int proto,
		struct in6_addr *src_addr, struct in6_addr *dest_addr,
		int src_port, int dest_port,
		unsigned int ipv)
{
	char mac[ETH_ALEN]={0}; //dummy
	if (ipv == 4)
		ecm_classifier_pcc_permit_accel_v4(mac, src_addr->s6_addr32[0], src_port, 
				mac, dest_addr->s6_addr32[0], dest_port, 
				proto);
	else
		ecm_classifier_pcc_permit_accel_v6(mac, src_addr, src_port,
				mac, dest_addr, dest_port,
				proto);

}

static void flush_rules(void){
	struct ecm_pcc_test_rule *rule = NULL;
	struct ecm_pcc_test_rule *tmp = NULL;
	struct list_head *pos, *q;

	list_for_each_entry(rule , &ecm_pcc_test_rules, list) {
		ecm_pcc_test_permit_accel(rule->proto, &rule->src_addr, &rule->dest_addr, rule->src_port, rule->dest_port, rule->ipv);
		// and reverse
		ecm_pcc_test_permit_accel(rule->proto, &rule->dest_addr, &rule->src_addr, rule->dest_port, rule->src_port, rule->ipv);
	}

	spin_lock_bh(&ecm_pcc_test_rules_lock);
	list_for_each_safe(pos, q , &ecm_pcc_test_rules) {
		tmp = list_entry(pos, struct ecm_pcc_test_rule, list);
		list_del(pos);
		kfree(tmp);
	}
	spin_unlock_bh(&ecm_pcc_test_rules_lock);
}

/*
 * ecm_pcc_test_add_rule()
 *	Add a new rule, return 0 on failure.  Given accel the ecm is informed
 *	of permit/deny accel status.
 */
static unsigned int ecm_pcc_test_add_rule(
				unsigned int proto,
				struct in6_addr *src_addr,
				struct in6_addr *dest_addr,
				int src_port, int dest_port,
				unsigned int ipv)
{
	struct ecm_pcc_test_rule *new_rule;
	new_rule = kzalloc(sizeof(struct ecm_pcc_test_rule), GFP_ATOMIC);
	if (!new_rule)
		return 0;

	new_rule->accel = ECM_CLASSIFIER_PCC_RESULT_DENIED;
	new_rule->proto = proto;
	new_rule->src_port = src_port;
	new_rule->dest_port = dest_port;
	new_rule->src_addr = *src_addr;
	new_rule->dest_addr = *dest_addr;
	new_rule->ipv = ipv;
	INIT_LIST_HEAD(&new_rule->list);

	spin_lock_bh(&ecm_pcc_test_rules_lock);
	list_add(&new_rule->list, &ecm_pcc_test_rules);
	spin_unlock_bh(&ecm_pcc_test_rules_lock);

	ecm_pcc_test_deny_accel(proto, src_addr, dest_addr, src_port, dest_port, ipv);

	return 1;
}

/*
 * ecm_pcc_test_rule_write()
 *	Write a rule
 */
static ssize_t ecm_pcc_test_rule_write(struct file *file,
		const char __user *user_buf, size_t count, loff_t *ppos)
{
	char *rule_buf;
	int field_count;
	char *field_ptr;
	char *fields[RULE_FIELDS];
	unsigned int oper;
	unsigned int proto;
	int src_port;
	int dest_port;
	struct in6_addr src_addr = IN6ADDR_ANY_INIT;
	struct in6_addr dest_addr = IN6ADDR_ANY_INIT;
	unsigned int ipv;
	int hexformat=0;

	/*
	 * buf is formed as:
	 * [0]      [1]        [2]       [3]          [4]       
	 * <proto>,<src_addr>,<src_port>,<dest_addr>,<dest_port>/
	 * e.g.:
	 * echo "6,192.168.1.33,1234,10.10.10.10,80" > /sys/kernel/debug/ecm_pcc_test/rule
	 * cat /sys/kernel/debug/ecm_pcc_test/rule (shows all rules)
	 */
	rule_buf = kzalloc(count + 1, GFP_ATOMIC);
	if (!rule_buf)
		return -EINVAL;

	memcpy(rule_buf, user_buf, count);
	if (rule_buf[0]=='F'){
		flush_rules();
		kfree(rule_buf);
		return count;
	}
	/*
	 * Split the buffer into its fields
	 */
	field_count = 0;
	field_ptr = rule_buf;
	fields[field_count] = strsep(&field_ptr, ",");
	while (fields[field_count] != NULL) {
		/*pr_info("Field %d: %s\n", field_count, fields[field_count]);*/
		field_count++;
		if (field_count == RULE_FIELDS)
			break;

		fields[field_count] = strsep(&field_ptr, ",");
	}

	if (field_count != RULE_FIELDS) {
		pr_info("Invalid field count %d\n", field_count);
		kfree(rule_buf);
		return -EINVAL;
	}

	/*
	 * Convert fields
	 */
	if (strcmp(fields[0],"tcp")==0){
		proto=6;
	}else if (strcmp(fields[0],"udp")==0){
		proto=17;
	}else if (sscanf(fields[0], "%u", &proto) != 1)
		goto sscanf_read_error;
	if (strchr(fields[1],'.')==0 && strlen(fields[1])==8){
		hexformat=1;
	}
	if (sscanf(fields[2], hexformat?"%04x":"%d", &src_port) != 1)
		goto sscanf_read_error;
	src_port = htons(src_port);

	if (sscanf(fields[4], hexformat?"%04x":"%d", &dest_port) != 1)
		goto sscanf_read_error;
	dest_port = htons(dest_port);

	if (hexformat){
		uint8_t *sptr = (uint8_t *)(src_addr.s6_addr);
		uint8_t *dptr = (uint8_t *)(dest_addr.s6_addr);
		ipv=4;
		if (sscanf(fields[1], "%02hhx%02hhx%02hhx%02hhx", sptr,sptr+1,sptr+2,sptr+3) != 4)
			goto sscanf_read_error;
		if (sscanf(fields[3], "%02hhx%02hhx%02hhx%02hhx", dptr,dptr+1,dptr+2,dptr+3) != 4)
			goto sscanf_read_error;
	}else{
		ipv = ecm_pcc_test_str_to_ip(fields[1], &src_addr);
		if (ipv != ecm_pcc_test_str_to_ip(fields[3], &dest_addr)) {
			pr_info("Conflicting IP address types\n");
			goto sscanf_read_error;
		}
	}
	kfree(rule_buf);

	/*pr_info(
			"proto: %u\n"
			"%pI6:%d\n"
			"%pI6:%d\n"
			"ipv: %u\n",
			proto,
			&src_addr, ntohs(src_port),
			&dest_addr, ntohs(dest_port),
			ipv
			);*/

	if (!ecm_pcc_test_add_rule(proto,&src_addr, &dest_addr, src_port, dest_port, ipv))
		return -EINVAL;


	return count;

sscanf_read_error:
	pr_info("sscanf read error\n");
	kfree(rule_buf);
	return -EINVAL;
}

/*
 * ecm_pcc_test_rule_seq_show()
 */
static int ecm_pcc_test_rule_seq_show(struct seq_file *m, void *v)
{
	struct ecm_pcc_test_rule *rule;
	rule = list_entry(v, struct ecm_pcc_test_rule, list);
	seq_printf(m, "RULE:\n"
			"\tproto: %u\n"
			"\t"
			"\t%pI6:%u\n"
			"\t"
			"\t%pI6:%d\n"
			"\tipv: %u\n",
			rule->proto,
			&rule->src_addr,
			ntohs(rule->src_port),
			&rule->dest_addr,
			ntohs(rule->dest_port),
			rule->ipv
			);

	return 0;
}

/*
 * ecm_pcc_test_rule_seq_stop()
 */
static void ecm_pcc_test_rule_seq_stop(struct seq_file *p, void *v)
{
	spin_unlock_bh(&ecm_pcc_test_rules_lock);
}

/*
 * ecm_pcc_test_rule_seq_start()
 */
static void *ecm_pcc_test_rule_seq_start(struct seq_file *m, loff_t *_pos)
{
	spin_lock_bh(&ecm_pcc_test_rules_lock);
	return seq_list_start(&ecm_pcc_test_rules, *_pos);
}

/*
 * ecm_pcc_test_rule_seq_next()
 */
static void *ecm_pcc_test_rule_seq_next(struct seq_file *p, void *v,
					loff_t *pos)
{
	return seq_list_next(v, &ecm_pcc_test_rules, pos);
}

/*
 * DebugFS attributes.
 */
DEFINE_SIMPLE_ATTRIBUTE(ecm_pcc_test_unregister_fops,
			ecm_pcc_test_unregister_get_unregister,
			ecm_pcc_test_unregister_set_unregister, "%llu\n");

static const struct seq_operations ecm_pcc_test_rule_seq_ops = {
	.start = ecm_pcc_test_rule_seq_start,
	.next  = ecm_pcc_test_rule_seq_next,
	.stop  = ecm_pcc_test_rule_seq_stop,
	.show  = ecm_pcc_test_rule_seq_show,
};

/*
 * ecm_pcc_test_rule_open()
 */
static int ecm_pcc_test_rule_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &ecm_pcc_test_rule_seq_ops);
}

/*
 * ecm_pcc_test_rule_release()
 */
static int ecm_pcc_test_rule_release(struct inode *inode, struct file *file)
{
	return seq_release(inode, file);
}

static const struct file_operations ecm_pcc_test_rule_fops = {
	.owner		= THIS_MODULE,
	.open		= ecm_pcc_test_rule_open,
	.write		= ecm_pcc_test_rule_write,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= ecm_pcc_test_rule_release,
};

/*
 * ecm_pcc_test_init()
 */
static int __init ecm_pcc_test_init(void)
{
	int result;

	pr_info("Circle PCC INIT\n");

	/*
	 * Create entries in DebugFS for control functions
	 */
	ecm_pcc_test_dentry = debugfs_create_dir("circle_pcc", NULL);
	if (!ecm_pcc_test_dentry) {
		pr_info("Failed to create PCC directory entry\n");
		return -1;
	}
	if (!debugfs_create_file("unregister",
			S_IRUGO | S_IWUSR, ecm_pcc_test_dentry,
			NULL, &ecm_pcc_test_unregister_fops)) {
		pr_info("Failed to create ecm_pcc_test_unregister_fops\n");
		debugfs_remove_recursive(ecm_pcc_test_dentry);
		return -2;
	}
	if (!debugfs_create_file("rule",
			S_IRUGO | S_IWUSR, ecm_pcc_test_dentry,
			NULL, &ecm_pcc_test_rule_fops)) {
		pr_info("Failed to create ecm_pcc_test_rule_fops\n");
		debugfs_remove_recursive(ecm_pcc_test_dentry);
		return -3;
	}

	/*
	 * Create our registrant structure
	 */
	ecm_pcc_test_registrant = (struct ecm_classifier_pcc_registrant *)
			kzalloc(sizeof(struct ecm_classifier_pcc_registrant),
				GFP_ATOMIC | __GFP_NOWARN);
	if (!ecm_pcc_test_registrant) {
		pr_info("Circle PCC Failed to alloc registrant\n");
		debugfs_remove_recursive(ecm_pcc_test_dentry);
		return -4;
	}
	ecm_pcc_test_registrant->version = 1;
	ecm_pcc_test_registrant->this_module = THIS_MODULE;
	ecm_pcc_test_registrant->ref = ecm_pcc_test_registrant_ref;
	ecm_pcc_test_registrant->deref = ecm_pcc_test_registrant_deref;
	ecm_pcc_test_registrant->okay_to_accel_v4 =
						ecm_pcc_test_okay_to_accel_v4;
	ecm_pcc_test_registrant->okay_to_accel_v6 =
						ecm_pcc_test_okay_to_accel_v6;

	/*
	 * Register with the PCC Classifier. ECM classifier will take a ref for
	 * registrant.
	 */
	result = ecm_classifier_pcc_register(ecm_pcc_test_registrant);
	if (result != 0) {
		pr_info("Circle PCC registrant failed to register: %d\n", result);
		kfree(ecm_pcc_test_registrant);
		debugfs_remove_recursive(ecm_pcc_test_dentry);
		return -5;
	}

	pr_info("Circle ECM PCC registrant REGISTERED\n");

	return 0;
}

/*
 * ecm_pcc_test_exit()
 */
static void __exit ecm_pcc_test_exit(void)
{
	pr_info("Circle PCC EXIT\n");
	debugfs_remove_recursive(ecm_pcc_test_dentry);
}

module_init(ecm_pcc_test_init)
module_exit(ecm_pcc_test_exit)

MODULE_DESCRIPTION("Circle PCC");
#ifdef MODULE_LICENSE
MODULE_LICENSE("Dual BSD/GPL");
#endif

