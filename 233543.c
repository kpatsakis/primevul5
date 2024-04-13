upload_data( struct net_device  *dev,  unsigned  framelen,  unsigned  frameno,
	     unsigned  is_first,  u32  crc )
{
	struct net_local  *nl = (struct net_local *) dev->priv;

	int  frame_ok;

	if( is_first )
		nl->wait_frameno = frameno,
		nl->inppos = 0;

	if( nl->wait_frameno == frameno ) {

		if( nl->inppos + framelen  <=  ETHER_MAX_LEN )
			frame_ok = append_frame_to_pkt( dev, framelen, crc );

		/*
		 * if CRC is right but framelen incorrect then transmitter
		 * error was occurred... drop entire packet
		 */
		else if( (frame_ok = skip_tail( dev->base_addr, framelen, crc ))
			 != 0 )
			nl->wait_frameno = 0,
			nl->inppos = 0,
#ifdef CONFIG_SBNI_MULTILINE
			((struct net_local *) nl->master->priv)
				->stats.rx_errors++,
			((struct net_local *) nl->master->priv)
				->stats.rx_missed_errors++;
#else
			nl->stats.rx_errors++,
			nl->stats.rx_missed_errors++;
#endif
			/* now skip all frames until is_first != 0 */
	} else
		frame_ok = skip_tail( dev->base_addr, framelen, crc );

	if( is_first  &&  !frame_ok )
		/*
		 * Frame has been broken, but we had already stored
		 * is_first... Drop entire packet.
		 */
		nl->wait_frameno = 0,
#ifdef CONFIG_SBNI_MULTILINE
		((struct net_local *) nl->master->priv)->stats.rx_errors++,
		((struct net_local *) nl->master->priv)->stats.rx_crc_errors++;
#else
		nl->stats.rx_errors++,
		nl->stats.rx_crc_errors++;
#endif

	return  frame_ok;
}