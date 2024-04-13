static int sctp_bind_add(struct sock *sk, struct sockaddr *addrs,
		int addrlen)
{
	int err;

	lock_sock(sk);
	err = sctp_setsockopt_bindx(sk, addrs, addrlen, SCTP_BINDX_ADD_ADDR);
	release_sock(sk);
	return err;
}