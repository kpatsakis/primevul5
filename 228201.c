static inline struct sk_buff *tcp_write_queue_prev(const struct sock *sk,
						   const struct sk_buff *skb)
{
	return skb_queue_prev(&sk->sk_write_queue, skb);
}