static inline void skb_set_tcp_pure_ack(struct sk_buff *skb)
{
	skb->truesize = 2;
}