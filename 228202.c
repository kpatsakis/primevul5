static inline u32 tcp_min_rtt(const struct tcp_sock *tp)
{
	return tp->rtt_min[0].rtt;
}