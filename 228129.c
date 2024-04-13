static inline void tcp_disable_early_retrans(struct tcp_sock *tp)
{
	tp->do_early_retrans = 0;
}