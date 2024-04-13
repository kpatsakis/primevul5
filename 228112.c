static inline void tcp_synq_overflow(const struct sock *sk)
{
	unsigned long last_overflow = tcp_sk(sk)->rx_opt.ts_recent_stamp;
	unsigned long now = jiffies;

	if (time_after(now, last_overflow + HZ))
		tcp_sk(sk)->rx_opt.ts_recent_stamp = now;
}