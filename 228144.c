static inline bool tcp_synq_no_recent_overflow(const struct sock *sk)
{
	unsigned long last_overflow = tcp_sk(sk)->rx_opt.ts_recent_stamp;

	return time_after(jiffies, last_overflow + TCP_SYNCOOKIE_VALID);
}