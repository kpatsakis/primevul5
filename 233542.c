sbni_interrupt( int  irq,  void  *dev_id )
{
	struct net_device	  *dev = dev_id;
	struct net_local  *nl  = dev->priv;
	int	repeat;

	spin_lock( &nl->lock );
	if( nl->second )
		spin_lock( &((struct net_local *) nl->second->priv)->lock );

	do {
		repeat = 0;
		if( inb( dev->base_addr + CSR0 ) & (RC_RDY | TR_RDY) )
			handle_channel( dev ),
			repeat = 1;
		if( nl->second  && 	/* second channel present */
		    (inb( nl->second->base_addr+CSR0 ) & (RC_RDY | TR_RDY)) )
			handle_channel( nl->second ),
			repeat = 1;
	} while( repeat );

	if( nl->second )
		spin_unlock( &((struct net_local *)nl->second->priv)->lock );
	spin_unlock( &nl->lock );
	return IRQ_HANDLED;
}