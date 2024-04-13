isdn_net_ciscohdlck_slarp_send_request(isdn_net_local *lp)
{
	struct sk_buff *skb;
	unsigned char *p;

	skb = isdn_net_ciscohdlck_alloc_skb(lp, 4 + 14);
	if (!skb)
		return;

	p = skb_put(skb, 4 + 14);

	/* cisco header */
	p += put_u8 (p, CISCO_ADDR_UNICAST);
	p += put_u8 (p, CISCO_CTRL);
	p += put_u16(p, CISCO_TYPE_SLARP);

	/* slarp request */
	p += put_u32(p, CISCO_SLARP_REQUEST);
	p += put_u32(p, 0); // address
	p += put_u32(p, 0); // netmask
	p += put_u16(p, 0); // unused

	isdn_net_write_super(lp, skb);
}