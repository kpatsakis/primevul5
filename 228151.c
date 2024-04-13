static inline bool skb_is_tcp_pure_ack(const struct sk_buff *skb)
{
	return skb->truesize == 2;
}