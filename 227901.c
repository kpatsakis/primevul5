static inline int tcp_head_timedout(const struct sock *sk)
{
	const struct tcp_sock *tp = tcp_sk(sk);

	return tp->packets_out &&
	       tcp_skb_timedout(sk, tcp_write_queue_head(sk));
}