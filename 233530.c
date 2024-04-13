prepare_to_send( struct sk_buff  *skb,  struct net_device  *dev )
{
	struct net_local  *nl = (struct net_local *) dev->priv;

	unsigned int  len;

	/* nl->tx_buf_p == NULL here! */
	if( nl->tx_buf_p )
		printk( KERN_ERR "%s: memory leak!\n", dev->name );

	nl->outpos = 0;
	nl->state &= ~(FL_WAIT_ACK | FL_NEED_RESEND);

	len = skb->len;
	if( len < SBNI_MIN_LEN )
		len = SBNI_MIN_LEN;

	nl->tx_buf_p	= skb;
	nl->tx_frameno	= (len + nl->maxframe - 1) / nl->maxframe;
	nl->framelen	= len < nl->maxframe  ?  len  :  nl->maxframe;

	outb( inb( dev->base_addr + CSR0 ) | TR_REQ,  dev->base_addr + CSR0 );
#ifdef CONFIG_SBNI_MULTILINE
	nl->master->trans_start = jiffies;
#else
	dev->trans_start = jiffies;
#endif
}