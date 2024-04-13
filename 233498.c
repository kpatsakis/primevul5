isdn_net_rebuild_header(struct sk_buff *skb)
{
	struct net_device *dev = skb->dev;
	isdn_net_local *lp = dev->priv;
	int ret = 0;

	if (lp->p_encap == ISDN_NET_ENCAP_ETHER) {
		struct ethhdr *eth = (struct ethhdr *) skb->data;

		/*
		 *      Only ARP/IP is currently supported
		 */

		if (eth->h_proto != htons(ETH_P_IP)) {
			printk(KERN_WARNING
			       "isdn_net: %s don't know how to resolve type %d addresses?\n",
			       dev->name, (int) eth->h_proto);
			memcpy(eth->h_source, dev->dev_addr, dev->addr_len);
			return 0;
		}
		/*
		 *      Try to get ARP to resolve the header.
		 */
#ifdef CONFIG_INET
		ret = arp_find(eth->h_dest, skb);
#endif
	}
	return ret;
}