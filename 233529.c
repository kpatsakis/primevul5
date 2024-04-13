indicate_pkt( struct net_device  *dev )
{
	struct net_local  *nl  = (struct net_local *) dev->priv;
	struct sk_buff    *skb = nl->rx_buf_p;

	skb_put( skb, nl->inppos );

#ifdef CONFIG_SBNI_MULTILINE
	skb->protocol = eth_type_trans( skb, nl->master );
	netif_rx( skb );
	dev->last_rx = jiffies;
	++((struct net_local *) nl->master->priv)->stats.rx_packets;
	((struct net_local *) nl->master->priv)->stats.rx_bytes += nl->inppos;
#else
	skb->protocol = eth_type_trans( skb, dev );
	netif_rx( skb );
	dev->last_rx = jiffies;
	++nl->stats.rx_packets;
	nl->stats.rx_bytes += nl->inppos;
#endif
	nl->rx_buf_p = NULL;	/* protocol driver will clear this sk_buff */
}