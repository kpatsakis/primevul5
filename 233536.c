card_start( struct net_device  *dev )
{
	struct net_local  *nl = (struct net_local *) dev->priv;

	nl->timer_ticks = CHANGE_LEVEL_START_TICKS;
	nl->state &= ~(FL_WAIT_ACK | FL_NEED_RESEND);
	nl->state |= FL_PREV_OK;

	nl->inppos = nl->outpos = 0;
	nl->wait_frameno = 0;
	nl->tx_frameno	 = 0;
	nl->framelen	 = 0;

	outb( *(u_char *)&nl->csr1 | PR_RES, dev->base_addr + CSR1 );
	outb( EN_INT, dev->base_addr + CSR0 );
}