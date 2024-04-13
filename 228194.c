static inline __u32 cookie_init_sequence(const struct tcp_request_sock_ops *ops,
					 const struct sock *sk, struct sk_buff *skb,
					 __u16 *mss)
{
	tcp_synq_overflow(sk);
	__NET_INC_STATS(sock_net(sk), LINUX_MIB_SYNCOOKIESSENT);
	return ops->cookie_init_seq(skb, mss);
}