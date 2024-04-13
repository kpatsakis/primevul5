get_rx_buf( struct net_device  *dev )
{
	/* +2 is to compensate for the alignment fixup below */
	struct sk_buff  *skb = dev_alloc_skb( ETHER_MAX_LEN + 2 );
	if( !skb )
		return  NULL;

	skb_reserve( skb, 2 );		/* Align IP on longword boundaries */
	return  skb;
}