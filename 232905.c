static int udp_lib_lport_inuse(struct net *net, __u16 num,
			       const struct udp_hslot *hslot,
			       unsigned long *bitmap,
			       struct sock *sk, unsigned int log)
{
	struct sock *sk2;
	kuid_t uid = sock_i_uid(sk);

	sk_for_each(sk2, &hslot->head) {
		if (net_eq(sock_net(sk2), net) &&
		    sk2 != sk &&
		    (bitmap || udp_sk(sk2)->udp_port_hash == num) &&
		    (!sk2->sk_reuse || !sk->sk_reuse) &&
		    (!sk2->sk_bound_dev_if || !sk->sk_bound_dev_if ||
		     sk2->sk_bound_dev_if == sk->sk_bound_dev_if) &&
		    inet_rcv_saddr_equal(sk, sk2, true)) {
			if (sk2->sk_reuseport && sk->sk_reuseport &&
			    !rcu_access_pointer(sk->sk_reuseport_cb) &&
			    uid_eq(uid, sock_i_uid(sk2))) {
				if (!bitmap)
					return 0;
			} else {
				if (!bitmap)
					return 1;
				__set_bit(udp_sk(sk2)->udp_port_hash >> log,
					  bitmap);
			}
		}
	}
	return 0;
}