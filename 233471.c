isdn_net_ciscohdlck_slarp_send_keepalive(unsigned long data)
{
	isdn_net_local *lp = (isdn_net_local *) data;
	struct sk_buff *skb;
	unsigned char *p;
	unsigned long last_cisco_myseq = lp->cisco_myseq;
	int myseq_diff = 0;

	if (!(lp->flags & ISDN_NET_CONNECTED) || lp->dialstate) {
		printk("isdn BUG at %s:%d!\n", __FILE__, __LINE__);
		return;
	}
	lp->cisco_myseq++;

	myseq_diff = (lp->cisco_myseq - lp->cisco_mineseen);
	if ((lp->cisco_line_state) && ((myseq_diff >= 3)||(myseq_diff <= -3))) {
		/* line up -> down */
		lp->cisco_line_state = 0;
		printk (KERN_WARNING
				"UPDOWN: Line protocol on Interface %s,"
				" changed state to down\n", lp->netdev->dev->name);
		/* should stop routing higher-level data accross */
	} else if ((!lp->cisco_line_state) &&
		(myseq_diff >= 0) && (myseq_diff <= 2)) {
		/* line down -> up */
		lp->cisco_line_state = 1;
		printk (KERN_WARNING
				"UPDOWN: Line protocol on Interface %s,"
				" changed state to up\n", lp->netdev->dev->name);
		/* restart routing higher-level data accross */
	}

	if (lp->cisco_debserint)
		printk (KERN_DEBUG "%s: HDLC "
			"myseq %lu, mineseen %lu%c, yourseen %lu, %s\n",
			lp->netdev->dev->name, last_cisco_myseq, lp->cisco_mineseen,
			((last_cisco_myseq == lp->cisco_mineseen) ? '*' : 040),
			lp->cisco_yourseq,
			((lp->cisco_line_state) ? "line up" : "line down"));

	skb = isdn_net_ciscohdlck_alloc_skb(lp, 4 + 14);
	if (!skb)
		return;

	p = skb_put(skb, 4 + 14);

	/* cisco header */
	p += put_u8 (p, CISCO_ADDR_UNICAST);
	p += put_u8 (p, CISCO_CTRL);
	p += put_u16(p, CISCO_TYPE_SLARP);

	/* slarp keepalive */
	p += put_u32(p, CISCO_SLARP_KEEPALIVE);
	p += put_u32(p, lp->cisco_myseq);
	p += put_u32(p, lp->cisco_yourseq);
	p += put_u16(p, 0xffff); // reliablity, always 0xffff

	isdn_net_write_super(lp, skb);

	lp->cisco_timer.expires = jiffies + lp->cisco_keepalive_period * HZ;
	
	add_timer(&lp->cisco_timer);
}