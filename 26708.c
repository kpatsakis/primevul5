static int sco_sock_bind(struct socket *sock, struct sockaddr *addr,
			 int addr_len)
{
	struct sockaddr_sco *sa = (struct sockaddr_sco *) addr;
	struct sock *sk = sock->sk;
	int err = 0;

	BT_DBG("sk %p %pMR", sk, &sa->sco_bdaddr);

 	if (!addr || addr->sa_family != AF_BLUETOOTH)
 		return -EINVAL;
 
 	lock_sock(sk);
 
 	if (sk->sk_state != BT_OPEN) {
		err = -EBADFD;
		goto done;
	}

	if (sk->sk_type != SOCK_SEQPACKET) {
		err = -EINVAL;
		goto done;
	}

	bacpy(&sco_pi(sk)->src, &sa->sco_bdaddr);

	sk->sk_state = BT_BOUND;

done:
	release_sock(sk);
	return err;
}
