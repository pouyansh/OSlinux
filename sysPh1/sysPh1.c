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
		if (adap == NULL) printk ("nulle\n");
		skb = adap->ourSkb;
		/*for (i = 1; i <= sizeof (adap->rx_ring)/sizeof (adap->rx_ring[0]); i++) {
			skb = adap->rx_ring [i].rx_skb_top; */
			while (skb != NULL) {
				if (n==1) {
					printk("%d: length = %u\n", k, skb->len);
					k = k+1;
				} 
				else if (n==2) {
					unsigned char *prot = skb_network_header(skb);
					printk("%d: protocol = %u, protocol in header = %u\n", k, skb->protocol, prot[9]);
					if (skb->protocol == 8) {
						if (prot[9]==17) printk(" udp ");
						else printk(" tcp ");
					}
					printk("\n");
					k = k+1;
					/*struct iphdr *ip_header; 
					ip_header = (struct iphdr *)skb_network_header(skb); 
					if (ip_header != NULL) {
						printk ("%d ", k); 
						if (ip_header->protocol== IPPROTO_TCP)
							printk("tcp "); 
						if (ip_header->protocol==IPPROTO_UDP)
							printk("udp "); 
						printk("\n");
						k = k+1;
					}*/
				}
				else if (n==3 ) {
					printk("%d: hash = %u\n", k, skb_get_hash(skb)); 
					k = k+1;
				}
				else if (n==4 ) {
					printk("%d: process time = %lld, start time = %lld",k, ktime_to_ms(skb->tstamp), ktime_to_ms(skb->starttime));
					if (skb->endtime) {
						printk("end time = %lld",ktime_to_ms(skb->endtime));
					}
					printk("\n");
					/*printk("%d\n", ktime_to_ms(ktime_get_real()));
					skb_get_timestamp (skb, tv); 
					k = k+1;
					if (tv) {
						printk("time = %d", (int) tv->tv_sec); 
						tv = NULL;
						k = k+1;
					}
					printk ("\n");*/
					k = k+1;
				}
				skb = skb->next;
			}
		//}
		if (n==5){
			adap->ourSkb = NULL;	
		}
		dev = next_net_device(dev);
	}
	read_unlock(&dev_base_lock);
	return 0;
}
