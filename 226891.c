static int sctp_bind(struct sock *sk, struct sockaddr *addr, int addr_len)
{
	int retval = 0;

	lock_sock(sk);

	pr_debug("%s: sk:%p, addr:%p, addr_len:%d\n", __func__, sk,
		 addr, addr_len);

	/* Disallow binding twice. */
	if (!sctp_sk(sk)->ep->base.bind_addr.port)
		retval = sctp_do_bind(sk, (union sctp_addr *)addr,
				      addr_len);
	else
		retval = -EINVAL;

	release_sock(sk);

	return retval;
}