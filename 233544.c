send_complete( struct net_local  *nl )
{
#ifdef CONFIG_SBNI_MULTILINE
	((struct net_local *) nl->master->priv)->stats.tx_packets++;
	((struct net_local *) nl->master->priv)->stats.tx_bytes
		+= nl->tx_buf_p->len;
#else
	nl->stats.tx_packets++;
	nl->stats.tx_bytes += nl->tx_buf_p->len;
#endif
	dev_kfree_skb_irq( nl->tx_buf_p );

	nl->tx_buf_p = NULL;

	nl->outpos = 0;
	nl->state &= ~(FL_WAIT_ACK | FL_NEED_RESEND);
	nl->framelen   = 0;
}