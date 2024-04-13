timeout_change_level( struct net_device  *dev )
{
	struct net_local  *nl = (struct net_local *) dev->priv;

	nl->cur_rxl_index = timeout_rxl_tab[ nl->timeout_rxl ];
	if( ++nl->timeout_rxl >= 4 )
		nl->timeout_rxl = 0;

	nl->csr1.rxl = rxl_tab[ nl->cur_rxl_index ];
	inb( dev->base_addr + CSR0 );
	outb( *(unsigned char *)&nl->csr1, dev->base_addr + CSR1 );

	nl->prev_rxl_rcvd = nl->cur_rxl_rcvd;
	nl->cur_rxl_rcvd  = 0;
}