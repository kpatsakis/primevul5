static inline void tipc_crypto_clone_msg(struct net *net, struct sk_buff *_skb,
					 struct tipc_bearer *b,
					 struct tipc_media_addr *dst,
					 struct tipc_node *__dnode, u8 type)
{
	struct sk_buff *skb;

	skb = skb_clone(_skb, GFP_ATOMIC);
	if (skb) {
		TIPC_SKB_CB(skb)->xmit_type = type;
		tipc_crypto_xmit(net, &skb, b, dst, __dnode);
		if (skb)
			b->media->send_msg(net, skb, b, dst);
	}
}