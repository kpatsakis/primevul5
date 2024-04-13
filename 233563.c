send_frame_header( struct net_device  *dev,  u32  *crc_p )
{
	struct net_local  *nl  = (struct net_local *) dev->priv;

	u32  crc = *crc_p;
	u32  len_field = nl->framelen + 6;	/* CRC + frameno + reserved */
	u8   value;

	if( nl->state & FL_NEED_RESEND )
		len_field |= FRAME_RETRY;	/* non-first attempt... */

	if( nl->outpos == 0 )
		len_field |= FRAME_FIRST;

	len_field |= (nl->state & FL_PREV_OK) ? FRAME_SENT_OK : FRAME_SENT_BAD;
	outb( SBNI_SIG, dev->base_addr + DAT );

	value = (u8) len_field;
	outb( value, dev->base_addr + DAT );
	crc = CRC32( value, crc );
	value = (u8) (len_field >> 8);
	outb( value, dev->base_addr + DAT );
	crc = CRC32( value, crc );

	outb( nl->tx_frameno, dev->base_addr + DAT );
	crc = CRC32( nl->tx_frameno, crc );
	outb( 0, dev->base_addr + DAT );
	crc = CRC32( 0, crc );
	*crc_p = crc;
}