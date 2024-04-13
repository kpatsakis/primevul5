static void prb_setup_retire_blk_timer(struct packet_sock *po)
{
	struct tpacket_kbdq_core *pkc;

	pkc = GET_PBDQC_FROM_RB(&po->rx_ring);
	timer_setup(&pkc->retire_blk_timer, prb_retire_rx_blk_timer_expired,
		    0);
	pkc->retire_blk_timer.expires = jiffies;
}