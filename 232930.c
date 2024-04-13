struct sock *udp4_lib_lookup(struct net *net, __be32 saddr, __be16 sport,
			     __be32 daddr, __be16 dport, int dif)
{
	struct sock *sk;

	sk = __udp4_lib_lookup(net, saddr, sport, daddr, dport,
			       dif, &udp_table, NULL);
	if (sk && !refcount_inc_not_zero(&sk->sk_refcnt))
		sk = NULL;
	return sk;
}