static inline int tcp_v6_iif(const struct sk_buff *skb)
{
	bool l3_slave = skb_l3mdev_slave(TCP_SKB_CB(skb)->header.h6.flags);

	return l3_slave ? skb->skb_iif : TCP_SKB_CB(skb)->header.h6.iif;
}