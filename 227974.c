static inline void TCP_ECN_accept_cwr(struct tcp_sock *tp, const struct sk_buff *skb)
{
	if (tcp_hdr(skb)->cwr)
		tp->ecn_flags &= ~TCP_ECN_DEMAND_CWR;
}