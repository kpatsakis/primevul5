struct sctp_chunk *sctp_chunkify(struct sk_buff *skb,
			    const struct sctp_association *asoc,
			    struct sock *sk)
{
	struct sctp_chunk *retval;

	retval = kmem_cache_zalloc(sctp_chunk_cachep, GFP_ATOMIC);

	if (!retval)
		goto nodata;

	if (!sk) {
		SCTP_DEBUG_PRINTK("chunkifying skb %p w/o an sk\n", skb);
	}

	INIT_LIST_HEAD(&retval->list);
	retval->skb		= skb;
	retval->asoc		= (struct sctp_association *)asoc;
	retval->resent  	= 0;
	retval->has_tsn		= 0;
	retval->has_ssn         = 0;
	retval->rtt_in_progress	= 0;
	retval->sent_at		= 0;
	retval->singleton	= 1;
	retval->end_of_packet	= 0;
	retval->ecn_ce_done	= 0;
	retval->pdiscard	= 0;

	/* sctpimpguide-05.txt Section 2.8.2
	 * M1) Each time a new DATA chunk is transmitted
	 * set the 'TSN.Missing.Report' count for that TSN to 0. The
	 * 'TSN.Missing.Report' count will be used to determine missing chunks
	 * and when to fast retransmit.
	 */
	retval->tsn_missing_report = 0;
	retval->tsn_gap_acked = 0;
	retval->fast_retransmit = 0;

	/* If this is a fragmented message, track all fragments
	 * of the message (for SEND_FAILED).
	 */
	retval->msg = NULL;

	/* Polish the bead hole.  */
	INIT_LIST_HEAD(&retval->transmitted_list);
	INIT_LIST_HEAD(&retval->frag_list);
	SCTP_DBG_OBJCNT_INC(chunk);
	atomic_set(&retval->refcnt, 1);

nodata:
	return retval;
}