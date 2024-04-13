static inline unsigned long tcp_probe0_base(const struct sock *sk)
{
	return max_t(unsigned long, inet_csk(sk)->icsk_rto, TCP_RTO_MIN);
}