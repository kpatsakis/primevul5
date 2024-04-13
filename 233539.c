emancipate( struct net_device  *dev )
{
	struct net_local   *snl = (struct net_local *) dev->priv;
	struct net_device  *p   = snl->master;
	struct net_local   *nl  = (struct net_local *) p->priv;

	if( !(snl->state & FL_SLAVE) )
		return  -EINVAL;

	spin_lock( &nl->lock );
	spin_lock( &snl->lock );
	drop_xmit_queue( dev );

	/* exclude from list */
	for(;;) {	/* must be in list */
		struct net_local  *t = (struct net_local *) p->priv;
		if( t->link == dev ) {
			t->link = snl->link;
			break;
		}
		p = t->link;
	}

	snl->link = NULL;
	snl->master = dev;
	snl->state &= ~FL_SLAVE;

	netif_start_queue( dev );

	spin_unlock( &snl->lock );
	spin_unlock( &nl->lock );

	dev_put( dev );
	return  0;
}