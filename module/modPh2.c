#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/can.h>
#include <linux/syscalls.h>
#include "e1000/e1000.h"
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

static int n = 1; 
static char in [5]; 
static char str2[1234]; 
MODULE_LICENSE ("GPL");

static int showin(struct seq_file *m, void *v){
	printk("showin\n");
	return 0;
}

static int showout (struct seq_file *m, void *v) {
	printk("showout\n");
	seq_printf (m,"%s\n", str2); 
	return 0;
}

static ssize_t writein(struct file* file,const char __user *buffer, size_t count,loff_t *f_pos) {
	printk("writeout\n");
	int k=1, i;
	struct net_device *dev;
	struct sk_buff *skb;
	struct e1000_adapter* adap;
	struct timeval *tv = NULL;
	
	copy_from_user(in, buffer, count<1234?count : 1234); 
	if (in [0] ==1) n=1; 
	if (in [0] ==2) n=2; 
	if (in [0] ==3) n=3;
	if (in [0] ==4) n=4;

	read_lock(&dev_base_lock);
	printk("1\n");
	dev = first_net_device(&init_net);
	printk("2\n");

	while (dev != NULL) {
		printk ("3\n"); 
		adap = (struct e1000_adapter*) netdev_priv(dev); 
		if (adap == NULL) printk ("nulle\n"); 
		for (i = 1; i <= sizeof (adap->rx_ring)/sizeof (adap->rx_ring[0]); i++) {
			skb = adap->rx_ring [i].rx_skb_top; 
			while (skb != NULL) {
				if (n==1) {
					printk("%d: length = %u\n", k, skb->len);
					k = k+1;
				} 
				else if (n==2) {
					struct iphdr *ip_header; 
					ip_header = (struct iphdr *)skb_network_header(skb); 
					if (ip_header != NULL) {
						printk ("%d ", k); 
						if (ip_header->protocol== IPPROTO_TCP)
							printk("tcp "); 
						if (ip_header->protocol==IPPROTO_UDP)
							printk("udp "); 
						printk("\n");
						k = k+1;
					}
				}
				else if (n==3 ) {
					printk("%d: hash = %u\n", k, skb->hash); 
					k = k+1;
				}
				else if (n==4 ) {
					skb_get_timestamp (skb, tv); 
					if (tv != NULL) {
						printk("%d: time = %d\n", k, (int) tv->tv_sec); 
						tv = NULL;
						k = k+1;
					}
				}
				skb = skb->next;
			}
		}
		dev = next_net_device(dev);
	}
	read_unlock(&dev_base_lock);
	return 0;
}

static ssize_t writeout (struct file* file,const char __user *buffer, size_t count, loff_t *f_pos) {
	printk("writeout\n");
	return 0;
}

static int openin(struct inode *inode, struct file *file) {
	printk("openin\n");	
	return single_open(file, showin,NULL);
}

static int openout (struct inode *inode, struct file *file) {
	printk("openout\n");
	return single_open(file, showout ,NULL);
}

static struct file_operations fin={
	.owner = THIS_MODULE, 
	.open = openin, 
	.release = single_release, 
	.read = seq_read, 
	.llseek = seq_lseek, 
	.write = writein
};

static struct file_operations fout={
	.owner = THIS_MODULE, 
	.open = openout, 
	.release = single_release, 
	.read = seq_read, 
	.llseek = seq_lseek, 
	.write = writeout
};

int init_module (void) {
	struct proc_dir_entry *inf, *outf; 
	proc_mkdir("sockinfo", NULL); 
	inf = proc_create("sockinfo/in",0777,NULL, &fin); 
	outf = proc_create("sockinfo/out",0777, NULL, &fout);
	if (!inf || !outf){
		return -1;
	}else{
		printk(KERN_INFO "success\n");
	}

	return 0;
}

void cleanup_module(void){
	//remove_proc_entry("sockinfo/out", NULL);
	//remove_proc_entry("sockinfo/in", NULL);
	//remove_proc_entry("sockinfo",NULL);
}
