static void prb_retire_rx_blk_timer_expired(struct timer_list *t)
{
	struct packet_sock *po =
		from_timer(po, t, rx_ring.prb_bdqc.retire_blk_timer);
	struct tpacket_kbdq_core *pkc = GET_PBDQC_FROM_RB(&po->rx_ring);
	unsigned int frozen;
	struct tpacket_block_desc *pbd;

	spin_lock(&po->sk.sk_receive_queue.lock);

	frozen = prb_queue_frozen(pkc);
	pbd = GET_CURR_PBLOCK_DESC_FROM_CORE(pkc);

	if (unlikely(pkc->delete_blk_timer))
		goto out;

	/* We only need to plug the race when the block is partially filled.
	 * tpacket_rcv:
	 *		lock(); increment BLOCK_NUM_PKTS; unlock()
	 *		copy_bits() is in progress ...
	 *		timer fires on other cpu:
	 *		we can't retire the current block because copy_bits
	 *		is in progress.
	 *
	 */
	if (BLOCK_NUM_PKTS(pbd)) {
		/* Waiting for skb_copy_bits to finish... */
		write_lock(&pkc->blk_fill_in_prog_lock);
		write_unlock(&pkc->blk_fill_in_prog_lock);
	}

	if (pkc->last_kactive_blk_num == pkc->kactive_blk_num) {
		if (!frozen) {
			if (!BLOCK_NUM_PKTS(pbd)) {
				/* An empty block. Just refresh the timer. */
				goto refresh_timer;
			}
			prb_retire_current_block(pkc, po, TP_STATUS_BLK_TMO);
			if (!prb_dispatch_next_block(pkc, po))
				goto refresh_timer;
			else
				goto out;
		} else {
			/* Case 1. Queue was frozen because user-space was
			 *	   lagging behind.
			 */
			if (prb_curr_blk_in_use(pbd)) {
				/*
				 * Ok, user-space is still behind.
				 * So just refresh the timer.
				 */
				goto refresh_timer;
			} else {
			       /* Case 2. queue was frozen,user-space caught up,
				* now the link went idle && the timer fired.
				* We don't have a block to close.So we open this
				* block and restart the timer.
				* opening a block thaws the queue,restarts timer
				* Thawing/timer-refresh is a side effect.
				*/
				prb_open_block(pkc, pbd);
				goto out;
			}
		}
	}

refresh_timer:
	_prb_refresh_rx_retire_blk_timer(pkc);

out:
	spin_unlock(&po->sk.sk_receive_queue.lock);
}