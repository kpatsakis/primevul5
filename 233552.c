recv_frame( struct net_device  *dev )
{
	struct net_local  *nl   = (struct net_local *) dev->priv;
	unsigned long  ioaddr	= dev->base_addr;

	u32  crc = CRC32_INITIAL;

	unsigned  framelen = 0, frameno, ack;
	unsigned  is_first, frame_ok = 0;

	if( check_fhdr( ioaddr, &framelen, &frameno, &ack, &is_first, &crc ) ) {
		frame_ok = framelen > 4
			?  upload_data( dev, framelen, frameno, is_first, crc )
			:  skip_tail( ioaddr, framelen, crc );
		if( frame_ok )
			interpret_ack( dev, ack );
	}

	outb( inb( ioaddr + CSR0 ) ^ CT_ZER, ioaddr + CSR0 );
	if( frame_ok ) {
		nl->state |= FL_PREV_OK;
		if( framelen > 4 )
			nl->in_stats.all_rx_number++;
	} else
		nl->state &= ~FL_PREV_OK,
		change_level( dev ),
		nl->in_stats.all_rx_number++,
		nl->in_stats.bad_rx_number++;

	return  !frame_ok  ||  framelen > 4;
}