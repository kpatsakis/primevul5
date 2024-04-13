static inline unsigned long tcp_probe0_when(const struct sock *sk,
					    unsigned long max_when)
{
	u64 when = (u64)tcp_probe0_base(sk) << inet_csk(sk)->icsk_backoff;

	return (unsigned long)min_t(u64, when, max_when);
}