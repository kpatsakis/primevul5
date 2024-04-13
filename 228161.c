static inline struct sk_buff *tcp_write_queue_next(const struct sock *sk,
						   const struct sk_buff *skb)
{
	return skb_queue_next(&sk->sk_write_queue, skb);
}