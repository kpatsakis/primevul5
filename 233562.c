drop_xmit_queue( struct net_device  *dev )
{
	struct net_local  *nl = (struct net_local *) dev->priv;

	if( nl->tx_buf_p )
		dev_kfree_skb_any( nl->tx_buf_p ),
		nl->tx_buf_p = NULL,
#ifdef CONFIG_SBNI_MULTILINE
		((struct net_local *) nl->master->priv)
			->stats.tx_errors++,
		((struct net_local *) nl->master->priv)
			->stats.tx_carrier_errors++;
#else
		nl->stats.tx_errors++,
		nl->stats.tx_carrier_errors++;
#endif

	nl->tx_frameno	= 0;
	nl->framelen	= 0;
	nl->outpos	= 0;
	nl->state &= ~(FL_WAIT_ACK | FL_NEED_RESEND);
#ifdef CONFIG_SBNI_MULTILINE
	netif_start_queue( nl->master );
	nl->master->trans_start = jiffies;
#else
	netif_start_queue( dev );
	dev->trans_start = jiffies;
#endif
}