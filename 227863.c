static int tcp_time_to_recover(struct sock *sk)
{
	struct tcp_sock *tp = tcp_sk(sk);
	__u32 packets_out;

	/* Do not perform any recovery during F-RTO algorithm */
	if (tp->frto_counter)
		return 0;

	/* Trick#1: The loss is proven. */
	if (tp->lost_out)
		return 1;

	/* Not-A-Trick#2 : Classic rule... */
	if (tcp_dupack_heuristics(tp) > tp->reordering)
		return 1;

	/* Trick#3 : when we use RFC2988 timer restart, fast
	 * retransmit can be triggered by timeout of queue head.
	 */
	if (tcp_is_fack(tp) && tcp_head_timedout(sk))
		return 1;

	/* Trick#4: It is still not OK... But will it be useful to delay
	 * recovery more?
	 */
	packets_out = tp->packets_out;
	if (packets_out <= tp->reordering &&
	    tp->sacked_out >= max_t(__u32, packets_out/2, sysctl_tcp_reordering) &&
	    !tcp_may_send_now(sk)) {
		/* We have nothing to send. This connection is limited
		 * either by receiver window or by application.
		 */
		return 1;
	}

	/* If a thin stream is detected, retransmit after first
	 * received dupack. Employ only if SACK is supported in order
	 * to avoid possible corner-case series of spurious retransmissions
	 * Use only if there are no unsent data.
	 */
	if ((tp->thin_dupack || sysctl_tcp_thin_dupack) &&
	    tcp_stream_is_thin(tp) && tcp_dupack_heuristics(tp) > 1 &&
	    tcp_is_sack(tp) && !tcp_send_head(sk))
		return 1;

	return 0;
}