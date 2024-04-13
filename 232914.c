static int udp_lib_lport_inuse2(struct net *net, __u16 num,
				struct udp_hslot *hslot2,
				struct sock *sk)
{
	struct sock *sk2;
	kuid_t uid = sock_i_uid(sk);
	int res = 0;

	spin_lock(&hslot2->lock);
	udp_portaddr_for_each_entry(sk2, &hslot2->head) {
		if (net_eq(sock_net(sk2), net) &&
		    sk2 != sk &&
		    (udp_sk(sk2)->udp_port_hash == num) &&
		    (!sk2->sk_reuse || !sk->sk_reuse) &&
		    (!sk2->sk_bound_dev_if || !sk->sk_bound_dev_if ||
		     sk2->sk_bound_dev_if == sk->sk_bound_dev_if) &&
		    inet_rcv_saddr_equal(sk, sk2, true)) {
			if (sk2->sk_reuseport && sk->sk_reuseport &&
			    !rcu_access_pointer(sk->sk_reuseport_cb) &&
			    uid_eq(uid, sock_i_uid(sk2))) {
				res = 0;
			} else {
				res = 1;
			}
			break;
		}
	}
	spin_unlock(&hslot2->lock);
	return res;
}