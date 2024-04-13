static void DBGUNDO(struct sock *sk, const char *msg)
{
	struct tcp_sock *tp = tcp_sk(sk);
	struct inet_sock *inet = inet_sk(sk);

	if (sk->sk_family == AF_INET) {
		printk(KERN_DEBUG "Undo %s %pI4/%u c%u l%u ss%u/%u p%u\n",
		       msg,
		       &inet->inet_daddr, ntohs(inet->inet_dport),
		       tp->snd_cwnd, tcp_left_out(tp),
		       tp->snd_ssthresh, tp->prior_ssthresh,
		       tp->packets_out);
	}
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	else if (sk->sk_family == AF_INET6) {
		struct ipv6_pinfo *np = inet6_sk(sk);
		printk(KERN_DEBUG "Undo %s %pI6/%u c%u l%u ss%u/%u p%u\n",
		       msg,
		       &np->daddr, ntohs(inet->inet_dport),
		       tp->snd_cwnd, tcp_left_out(tp),
		       tp->snd_ssthresh, tp->prior_ssthresh,
		       tp->packets_out);
	}
#endif
}