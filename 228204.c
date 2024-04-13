static inline void tcp_enable_early_retrans(struct tcp_sock *tp)
{
	struct net *net = sock_net((struct sock *)tp);

	tp->do_early_retrans = sysctl_tcp_early_retrans &&
		sysctl_tcp_early_retrans < 4 && !sysctl_tcp_thin_dupack &&
		net->ipv4.sysctl_tcp_reordering == 3;
}