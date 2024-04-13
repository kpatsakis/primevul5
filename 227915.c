static void tcp_enter_frto_loss(struct sock *sk, int allowed_segments, int flag)
{
	struct tcp_sock *tp = tcp_sk(sk);
	struct sk_buff *skb;

	tp->lost_out = 0;
	tp->retrans_out = 0;
	if (tcp_is_reno(tp))
		tcp_reset_reno_sack(tp);

	tcp_for_write_queue(skb, sk) {
		if (skb == tcp_send_head(sk))
			break;

		TCP_SKB_CB(skb)->sacked &= ~TCPCB_LOST;
		/*
		 * Count the retransmission made on RTO correctly (only when
		 * waiting for the first ACK and did not get it)...
		 */
		if ((tp->frto_counter == 1) && !(flag & FLAG_DATA_ACKED)) {
			/* For some reason this R-bit might get cleared? */
			if (TCP_SKB_CB(skb)->sacked & TCPCB_SACKED_RETRANS)
				tp->retrans_out += tcp_skb_pcount(skb);
			/* ...enter this if branch just for the first segment */
			flag |= FLAG_DATA_ACKED;
		} else {
			if (TCP_SKB_CB(skb)->sacked & TCPCB_RETRANS)
				tp->undo_marker = 0;
			TCP_SKB_CB(skb)->sacked &= ~TCPCB_SACKED_RETRANS;
		}

		/* Marking forward transmissions that were made after RTO lost
		 * can cause unnecessary retransmissions in some scenarios,
		 * SACK blocks will mitigate that in some but not in all cases.
		 * We used to not mark them but it was causing break-ups with
		 * receivers that do only in-order receival.
		 *
		 * TODO: we could detect presence of such receiver and select
		 * different behavior per flow.
		 */
		if (!(TCP_SKB_CB(skb)->sacked & TCPCB_SACKED_ACKED)) {
			TCP_SKB_CB(skb)->sacked |= TCPCB_LOST;
			tp->lost_out += tcp_skb_pcount(skb);
			tp->retransmit_high = TCP_SKB_CB(skb)->end_seq;
		}
	}
	tcp_verify_left_out(tp);

	tp->snd_cwnd = tcp_packets_in_flight(tp) + allowed_segments;
	tp->snd_cwnd_cnt = 0;
	tp->snd_cwnd_stamp = tcp_time_stamp;
	tp->frto_counter = 0;
	tp->bytes_acked = 0;

	tp->reordering = min_t(unsigned int, tp->reordering,
			       sysctl_tcp_reordering);
	tcp_set_ca_state(sk, TCP_CA_Loss);
	tp->high_seq = tp->snd_nxt;
	TCP_ECN_queue_cwr(tp);

	tcp_clear_all_retrans_hints(tp);
}