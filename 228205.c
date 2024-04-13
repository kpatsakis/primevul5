static inline int keepalive_time_when(const struct tcp_sock *tp)
{
	struct net *net = sock_net((struct sock *)tp);

	return tp->keepalive_time ? : net->ipv4.sysctl_tcp_keepalive_time;
}