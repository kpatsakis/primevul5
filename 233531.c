append_frame_to_pkt( struct net_device  *dev,  unsigned  framelen,  u32  crc )
{
	struct net_local  *nl = (struct net_local *) dev->priv;

	u8  *p;

	if( nl->inppos + framelen  >  ETHER_MAX_LEN )
		return  0;

	if( !nl->rx_buf_p  &&  !(nl->rx_buf_p = get_rx_buf( dev )) )
		return  0;

	p = nl->rx_buf_p->data + nl->inppos;
	insb( dev->base_addr + DAT, p, framelen );
	if( calc_crc32( crc, p, framelen ) != CRC32_REMAINDER )
		return  0;

	nl->inppos += framelen - 4;
	if( --nl->wait_frameno == 0 )		/* last frame received */
		indicate_pkt( dev );

	return  1;
}