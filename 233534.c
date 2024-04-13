sbni_start_xmit( struct sk_buff  *skb,  struct net_device  *dev )
{
	struct net_local  *nl  = (struct net_local *) dev->priv;

	netif_stop_queue( dev );
	spin_lock( &nl->lock );

	prepare_to_send( skb, dev );

	spin_unlock( &nl->lock );
	return  0;
}