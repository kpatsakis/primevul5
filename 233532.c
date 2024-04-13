sbni_watchdog( unsigned long  arg )
{
	struct net_device  *dev = (struct net_device *) arg;
	struct net_local   *nl  = (struct net_local *) dev->priv;
	struct timer_list  *w   = &nl->watchdog; 
	unsigned long	   flags;
	unsigned char	   csr0;

	spin_lock_irqsave( &nl->lock, flags );

	csr0 = inb( dev->base_addr + CSR0 );
	if( csr0 & RC_CHK ) {

		if( nl->timer_ticks ) {
			if( csr0 & (RC_RDY | BU_EMP) )
				/* receiving not active */
				nl->timer_ticks--;
		} else {
			nl->in_stats.timeout_number++;
			if( nl->delta_rxl )
				timeout_change_level( dev );

			outb( *(u_char *)&nl->csr1 | PR_RES,
			      dev->base_addr + CSR1 );
			csr0 = inb( dev->base_addr + CSR0 );
		}
	} else
		nl->state &= ~FL_LINE_DOWN;

	outb( csr0 | RC_CHK, dev->base_addr + CSR0 ); 

	init_timer( w );
	w->expires	= jiffies + SBNI_TIMEOUT;
	w->data		= arg;
	w->function	= sbni_watchdog;
	add_timer( w );

	spin_unlock_irqrestore( &nl->lock, flags );
}