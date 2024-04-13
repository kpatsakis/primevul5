static int tcp_is_sackfrto(const struct tcp_sock *tp)
{
	return (sysctl_tcp_frto == 0x2) && !tcp_is_reno(tp);
}