isdn_net_ciscohdlck_slarp_in(isdn_net_local *lp, struct sk_buff *skb)
{
	unsigned char *p;
	int period;
	u32 code;
	u32 my_seq, addr;
	u32 your_seq, mask;
	u32 local;
	u16 unused;

	if (skb->len < 14)
		return;

	p = skb->data;
	p += get_u32(p, &code);
	
	switch (code) {
	case CISCO_SLARP_REQUEST:
		lp->cisco_yourseq = 0;
		isdn_net_ciscohdlck_slarp_send_reply(lp);
		break;
	case CISCO_SLARP_REPLY:
		addr = ntohl(*(u32 *)p);
		mask = ntohl(*(u32 *)(p+4));
		if (mask != 0xfffffffc)
			goto slarp_reply_out;
		if ((addr & 3) == 0 || (addr & 3) == 3)
			goto slarp_reply_out;
		local = addr ^ 3;
		printk(KERN_INFO "%s: got slarp reply: "
			"remote ip: %d.%d.%d.%d, "
			"local ip: %d.%d.%d.%d "
			"mask: %d.%d.%d.%d\n",
		       lp->netdev->dev->name,
		       HIPQUAD(addr),
		       HIPQUAD(local),
		       HIPQUAD(mask));
		break;
  slarp_reply_out:
		 printk(KERN_INFO "%s: got invalid slarp "
				 "reply (%d.%d.%d.%d/%d.%d.%d.%d) "
				 "- ignored\n", lp->netdev->dev->name,
				 HIPQUAD(addr), HIPQUAD(mask));
		break;
	case CISCO_SLARP_KEEPALIVE:
		period = (int)((jiffies - lp->cisco_last_slarp_in
				+ HZ/2 - 1) / HZ);
		if (lp->cisco_debserint &&
				(period != lp->cisco_keepalive_period) &&
				lp->cisco_last_slarp_in) {
			printk(KERN_DEBUG "%s: Keepalive period mismatch - "
				"is %d but should be %d.\n",
				lp->netdev->dev->name, period,
				lp->cisco_keepalive_period);
		}
		lp->cisco_last_slarp_in = jiffies;
		p += get_u32(p, &my_seq);
		p += get_u32(p, &your_seq);
		p += get_u16(p, &unused);
		lp->cisco_yourseq = my_seq;
		lp->cisco_mineseen = your_seq;
		break;
	}
}