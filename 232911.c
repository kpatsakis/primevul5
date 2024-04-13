static bool udp_skb_has_head_state(struct sk_buff *skb)
{
	return !(udp_skb_scratch(skb)->_tsize_state & UDP_SKB_IS_STATELESS);
}