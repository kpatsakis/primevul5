sbni_close( struct net_device  *dev )
{
	struct net_local  *nl = (struct net_local *) dev->priv;

	if( nl->second  &&  nl->second->flags & IFF_UP ) {
		printk( KERN_NOTICE "Secondary channel (%s) is active!\n",
			nl->second->name );
		return  -EBUSY;
	}

#ifdef CONFIG_SBNI_MULTILINE
	if( nl->state & FL_SLAVE )
		emancipate( dev );
	else
		while( nl->link )	/* it's master device! */
			emancipate( nl->link );
#endif

	spin_lock( &nl->lock );

	nl->second = NULL;
	drop_xmit_queue( dev );	
	netif_stop_queue( dev );
   
	del_timer( &nl->watchdog );

	outb( 0, dev->base_addr + CSR0 );

	if( !(nl->state & FL_SECONDARY) )
		free_irq( dev->irq, dev );
	nl->state &= FL_SECONDARY;

	spin_unlock( &nl->lock );
	return 0;
}