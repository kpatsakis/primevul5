static inline int keepalive_intvl_when(const struct tcp_sock *tp)
{
	struct net *net = sock_net((struct sock *)tp);

	return tp->keepalive_intvl ? : net->ipv4.sysctl_tcp_keepalive_intvl;
}