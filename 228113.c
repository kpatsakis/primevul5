static inline int keepalive_probes(const struct tcp_sock *tp)
{
	struct net *net = sock_net((struct sock *)tp);

	return tp->keepalive_probes ? : net->ipv4.sysctl_tcp_keepalive_probes;
}