static void tpacket_destruct_skb(struct sk_buff *skb)
{
	struct packet_sock *po = pkt_sk(skb->sk);

	if (likely(po->tx_ring.pg_vec)) {
		void *ph;
		__u32 ts;

		ph = skb_zcopy_get_nouarg(skb);
		packet_dec_pending(&po->tx_ring);

		ts = __packet_set_timestamp(po, ph, skb);
		__packet_set_status(po, ph, TP_STATUS_AVAILABLE | ts);

		if (!packet_read_pending(&po->tx_ring))
			complete(&po->skb_completion);
	}

	sock_wfree(skb);
}