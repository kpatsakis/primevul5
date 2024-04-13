static inline u32 tcp_notsent_lowat(const struct tcp_sock *tp)
{
	struct net *net = sock_net((struct sock *)tp);
	return tp->notsent_lowat ?: net->ipv4.sysctl_tcp_notsent_lowat;
}