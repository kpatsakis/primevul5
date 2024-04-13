static inline bool tcp_skb_can_collapse_to(const struct sk_buff *skb)
{
	return likely(!TCP_SKB_CB(skb)->eor);
}