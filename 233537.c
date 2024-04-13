send_frame( struct net_device  *dev )
{
	struct net_local  *nl    = (struct net_local *) dev->priv;

	u32  crc = CRC32_INITIAL;

	if( nl->state & FL_NEED_RESEND ) {

		/* if frame was sended but not ACK'ed - resend it */
		if( nl->trans_errors ) {
			--nl->trans_errors;
			if( nl->framelen != 0 )
				nl->in_stats.resend_tx_number++;
		} else {
			/* cannot xmit with many attempts */
#ifdef CONFIG_SBNI_MULTILINE
			if( (nl->state & FL_SLAVE)  ||  nl->link )
#endif
			nl->state |= FL_LINE_DOWN;
			drop_xmit_queue( dev );
			goto  do_send;
		}
	} else
		nl->trans_errors = TR_ERROR_COUNT;

	send_frame_header( dev, &crc );
	nl->state |= FL_NEED_RESEND;
	/*
	 * FL_NEED_RESEND will be cleared after ACK, but if empty
	 * frame sended then in prepare_to_send next frame
	 */


	if( nl->framelen ) {
		download_data( dev, &crc );
		nl->in_stats.all_tx_number++;
		nl->state |= FL_WAIT_ACK;
	}

	outsb( dev->base_addr + DAT, (u8 *)&crc, sizeof crc );

do_send:
	outb( inb( dev->base_addr + CSR0 ) & ~TR_REQ, dev->base_addr + CSR0 );

	if( nl->tx_frameno )
		/* next frame exists - we request card to send it */
		outb( inb( dev->base_addr + CSR0 ) | TR_REQ,
		      dev->base_addr + CSR0 );
}