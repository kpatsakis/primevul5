static void prb_retire_current_block(struct tpacket_kbdq_core *pkc,
		struct packet_sock *po, unsigned int status)
{
	struct tpacket_block_desc *pbd = GET_CURR_PBLOCK_DESC_FROM_CORE(pkc);

	/* retire/close the current block */
	if (likely(TP_STATUS_KERNEL == BLOCK_STATUS(pbd))) {
		/*
		 * Plug the case where copy_bits() is in progress on
		 * cpu-0 and tpacket_rcv() got invoked on cpu-1, didn't
		 * have space to copy the pkt in the current block and
		 * called prb_retire_current_block()
		 *
		 * We don't need to worry about the TMO case because
		 * the timer-handler already handled this case.
		 */
		if (!(status & TP_STATUS_BLK_TMO)) {
			/* Waiting for skb_copy_bits to finish... */
			write_lock(&pkc->blk_fill_in_prog_lock);
			write_unlock(&pkc->blk_fill_in_prog_lock);
		}
		prb_close_block(pkc, pbd, po, status);
		return;
	}
}