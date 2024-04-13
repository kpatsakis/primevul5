static inline u32 tcp_skb_timestamp(const struct sk_buff *skb)
{
	return skb->skb_mstamp.stamp_jiffies;
}