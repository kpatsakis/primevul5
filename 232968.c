static int udp_reuseport_add_sock(struct sock *sk, struct udp_hslot *hslot)
{
	struct net *net = sock_net(sk);
	kuid_t uid = sock_i_uid(sk);
	struct sock *sk2;

	sk_for_each(sk2, &hslot->head) {
		if (net_eq(sock_net(sk2), net) &&
		    sk2 != sk &&
		    sk2->sk_family == sk->sk_family &&
		    ipv6_only_sock(sk2) == ipv6_only_sock(sk) &&
		    (udp_sk(sk2)->udp_port_hash == udp_sk(sk)->udp_port_hash) &&
		    (sk2->sk_bound_dev_if == sk->sk_bound_dev_if) &&
		    sk2->sk_reuseport && uid_eq(uid, sock_i_uid(sk2)) &&
		    inet_rcv_saddr_equal(sk, sk2, false)) {
			return reuseport_add_sock(sk, sk2);
		}
	}

	/* Initial allocation may have already happened via setsockopt */
	if (!rcu_access_pointer(sk->sk_reuseport_cb))
		return reuseport_alloc(sk);
	return 0;
}