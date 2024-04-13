static inline int tcp_skb_mss(const struct sk_buff *skb)
{
	return TCP_SKB_CB(skb)->tcp_gso_size;
}