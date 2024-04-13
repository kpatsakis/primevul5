static inline bool tcp_is_fack(const struct tcp_sock *tp)
{
	return tp->rx_opt.sack_ok & TCP_FACK_ENABLED;
}