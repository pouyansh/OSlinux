#include <linux/module.h>
#include <linux/kernel.h>
#include<linux/init.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/ethtool.h>
#include <linux/mii.h>

#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include<linux/fs.h>
#include<linux/seq_file.h>


#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include <linux/skbuff.h>

#include<linux/syscalls.h>
#include<config/e1000.h>

#include <asm/dma.h>
#include <asm/irq.h>

#include "e1000/e1000.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ma");
MODULE_DESCRIPTION("module");
MODULE_VERSION("1.0");

struct proc_dir_entry *subdirparent;
struct proc_dir_entry *parent;

static char *command = NULL;

static int showin(struct seq_file *m,void *v){
	printk("showin");
	seq_printf(m, "%s", command);
	return 0;
}

static int showout(struct seq_file *m,void *v){
	printk("showout");
	int k = 1;
	int n = (int)(command[0]) - (int)('0');
	struct sk_buff* skb;
	struct e1000_adapter* adap;
	struct net_device *dev;
	read_lock(&dev_base_lock);
	dev = first_net_device(&init_net);

	while (dev != NULL) {
		adap = (struct e1000_adapter*) netdev_priv(dev); 
		if (adap == NULL) printk ("null\n");
		else {
			skb = adap->ourSkb;
			while (skb != NULL) {
				if (n==1) {
					seq_printf(m,"%d: length = %u\n", k, skb->len);
					k = k+1;
				} 
				else if (n==2) {
					unsigned char *prot = skb_network_header(skb);
					seq_printf(m,"%d: protocol = %u ", k, skb->protocol);
					if (skb->protocol == 8) {
						if (prot[9]==17) seq_printf(m,"ethernet udp");
						else seq_printf(m,"ethernet tcp");
					}
					seq_printf(m,"\n");
					k = k+1;
				}
				else if (n==3 ) {
					seq_printf(m,"%d: hash = %u\n", k, skb_get_hash(skb)); 
					k = k+1;
				}
				else if (n==4 ) {
					seq_printf(m,"%d: start time = %lld",k, ktime_to_ms(skb->startTime));
					if (skb->endTime) {
						seq_printf(m,"end time = %lld",ktime_to_ms(skb->endTime));
					}
					seq_printf(m,"\n");
					k = k+1;
				}
				skb = skb->next;
				if (n==5){
					skb->prev = NULL;	
				}
			}
		}
		dev = next_net_device(dev);
	}
	read_unlock(&dev_base_lock);
	return 0;
}

static ssize_t writein(struct file* file,const char __user *buffer,size_t count,loff_t *f_pos){
	printk("writein");
	char *tmp = kzalloc((count+1),GFP_KERNEL);
	if(!tmp)return -ENOMEM;
	if(copy_from_user(tmp,buffer,count)){
		kfree(tmp);
		return EFAULT;
	}
	kfree(command);
	command=tmp;
	return count;
}

static int openout(struct inode *inode,struct file *file){
	return single_open(file,showout,NULL);
}

static int openin(struct inode *inode,struct file *file){
	return single_open(file,showin,NULL);
}

struct file_operations in_proc_fops = {
	.owner = THIS_MODULE,
	.open = openin,
	.release = single_release,
	.read = seq_read,
	.llseek = seq_lseek,
	.write = writein
};

struct file_operations out_proc_fops = {
	.owner = THIS_MODULE,
	.open = openout,
	.release = single_release,
	.read = seq_read,
	.llseek = seq_lseek,
};

static char *msg;

static void create_new_proc_entry(void)
{
	parent = proc_mkdir("sockinfo", NULL);
	proc_create("in",0777,parent,&in_proc_fops);
	proc_create("out",0777,parent,&out_proc_fops);
	msg=kmalloc(GFP_KERNEL,10*sizeof(char));
}


static int socinfo_mod_init (void)
{
	create_new_proc_entry();	
	return 0;
}

static void socinfo_mod_exit(void)
{
	//remove_proc_entry("in",parent);
	//remove_proc_entry("out",parent);
	//remove_proc_entry(dirname,NULL);
}


module_init(socinfo_mod_init);
module_exit(socinfo_mod_exit);
