isdn_net_ciscohdlck_slarp_send_reply(isdn_net_local *lp)
{
	struct sk_buff *skb;
	unsigned char *p;
	struct in_device *in_dev = NULL;
	__be32 addr = 0;		/* local ipv4 address */
	__be32 mask = 0;		/* local netmask */

	if ((in_dev = lp->netdev->dev->ip_ptr) != NULL) {
		/* take primary(first) address of interface */
		struct in_ifaddr *ifa = in_dev->ifa_list;
		if (ifa != NULL) {
			addr = ifa->ifa_local;
			mask = ifa->ifa_mask;
		}
	}

	skb = isdn_net_ciscohdlck_alloc_skb(lp, 4 + 14);
	if (!skb)
		return;

	p = skb_put(skb, 4 + 14);

	/* cisco header */
	p += put_u8 (p, CISCO_ADDR_UNICAST);
	p += put_u8 (p, CISCO_CTRL);
	p += put_u16(p, CISCO_TYPE_SLARP);

	/* slarp reply, send own ip/netmask; if values are nonsense remote
	 * should think we are unable to provide it with an address via SLARP */
	p += put_u32(p, CISCO_SLARP_REPLY);
	p += put_u32(p, addr);	// address
	p += put_u32(p, mask);	// netmask
	p += put_u16(p, 0);	// unused

	isdn_net_write_super(lp, skb);
}