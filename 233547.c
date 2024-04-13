change_level( struct net_device  *dev )
{
	struct net_local  *nl = (struct net_local *) dev->priv;

	if( nl->delta_rxl == 0 )	/* do not auto-negotiate RxL */
		return;

	if( nl->cur_rxl_index == 0 )
		nl->delta_rxl = 1;
	else if( nl->cur_rxl_index == 15 )
		nl->delta_rxl = -1;
	else if( nl->cur_rxl_rcvd < nl->prev_rxl_rcvd )
		nl->delta_rxl = -nl->delta_rxl;

	nl->csr1.rxl = rxl_tab[ nl->cur_rxl_index += nl->delta_rxl ];
	inb( dev->base_addr + CSR0 );	/* needs for PCI cards */
	outb( *(u8 *)&nl->csr1, dev->base_addr + CSR1 );

	nl->prev_rxl_rcvd = nl->cur_rxl_rcvd;
	nl->cur_rxl_rcvd  = 0;
}