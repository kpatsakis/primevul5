static inline void tcp_skb_pcount_add(struct sk_buff *skb, int segs)
{
	TCP_SKB_CB(skb)->tcp_gso_segs += segs;
}