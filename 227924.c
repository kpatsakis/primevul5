static inline void TCP_ECN_check_ce(struct tcp_sock *tp, const struct sk_buff *skb)
{
	if (!(tp->ecn_flags & TCP_ECN_OK))
		return;

	switch (TCP_SKB_CB(skb)->ip_dsfield & INET_ECN_MASK) {
	case INET_ECN_NOT_ECT:
		/* Funny extension: if ECT is not set on a segment,
		 * and we already seen ECT on a previous segment,
		 * it is probably a retransmit.
		 */
		if (tp->ecn_flags & TCP_ECN_SEEN)
			tcp_enter_quickack_mode((struct sock *)tp);
		break;
	case INET_ECN_CE:
		tp->ecn_flags |= TCP_ECN_DEMAND_CWR;
		/* fallinto */
	default:
		tp->ecn_flags |= TCP_ECN_SEEN;
	}
}