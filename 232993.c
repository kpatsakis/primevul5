static int udp_skb_truesize(struct sk_buff *skb)
{
	return udp_skb_scratch(skb)->_tsize_state & ~UDP_SKB_IS_STATELESS;
}