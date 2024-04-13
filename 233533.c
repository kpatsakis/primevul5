interpret_ack( struct net_device  *dev,  unsigned  ack )
{
	struct net_local  *nl = (struct net_local *) dev->priv;

	if( ack == FRAME_SENT_OK ) {
		nl->state &= ~FL_NEED_RESEND;

		if( nl->state & FL_WAIT_ACK ) {
			nl->outpos += nl->framelen;

			if( --nl->tx_frameno )
				nl->framelen = min_t(unsigned int,
						   nl->maxframe,
						   nl->tx_buf_p->len - nl->outpos);
			else
				send_complete( nl ),
#ifdef CONFIG_SBNI_MULTILINE
				netif_wake_queue( nl->master );
#else
				netif_wake_queue( dev );
#endif
		}
	}

	nl->state &= ~FL_WAIT_ACK;
}