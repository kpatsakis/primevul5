sbni_start_xmit( struct sk_buff  *skb,  struct net_device  *dev )
{
	struct net_device  *p;

	netif_stop_queue( dev );

	/* Looking for idle device in the list */
	for( p = dev;  p; ) {
		struct net_local  *nl = (struct net_local *) p->priv;
		spin_lock( &nl->lock );
		if( nl->tx_buf_p  ||  (nl->state & FL_LINE_DOWN) ) {
			p = nl->link;
			spin_unlock( &nl->lock );
		} else {
			/* Idle dev is found */
			prepare_to_send( skb, p );
			spin_unlock( &nl->lock );
			netif_start_queue( dev );
			return  0;
		}
	}

	return  1;
}