#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/can.h>
#include <linux/syscalls.h>
#include <../drivers/net/ethernet/intel/e1000/e1000.h>

asmlinkage long sys_sysPh1(int n){
	int k=1;
	struct net_device *dev;
	struct sk_buff *skb;
	struct e1000_adapter* adap;
	struct timeval *tv = NULL;

	read_lock(&dev_base_lock);
	dev = first_net_device(&init_net);

	while (dev != NULL) {
		adap = (struct e1000_adapter*) netdev_priv(dev); 
		if (adap == NULL) printk ("null\n");
		else {
			skb = adap->ourSkb;
			while (skb != NULL) {
				if (n==1) {
					printk("%d: length = %u\n", k, skb->len);
					k = k+1;
				} 
				else if (n==2) {
					unsigned char *prot = skb_network_header(skb);
					printk("%d: protocol = %u ", k, skb->protocol);
					if (skb->protocol == 8) {
						if (prot[9]==17) printk("ethernet udp");
						else printk("ethernet tcp");
					}
					printk("\n");
					k = k+1;
				}
				else if (n==3 ) {
					printk("%d: hash = %u\n", k, skb_get_hash(skb)); 
					k = k+1;
				}
				else if (n==4 ) {
					printk("%d: start time = %lld",k, ktime_to_ms(skb->startTime));
					if (skb->endTime) {
						printk("end time = %lld",ktime_to_ms(skb->endTime));
					}
					printk("\n");
					k = k+1;
				}
				skb = skb->next;
			}
			if (n==5){
				adap->ourSkb = NULL;	
			}
		}
		dev = next_net_device(dev);
	}
	read_unlock(&dev_base_lock);
	return 0;
}
