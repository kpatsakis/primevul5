static inline void tcp_segs_in(struct tcp_sock *tp, const struct sk_buff *skb)
{
	u16 segs_in;

	segs_in = max_t(u16, 1, skb_shinfo(skb)->gso_segs);
	tp->segs_in += segs_in;
	if (skb->len > tcp_hdrlen(skb))
		tp->data_segs_in += segs_in;
}