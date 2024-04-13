static int sctp_connect(struct sock *sk, struct sockaddr *addr,
			int addr_len, int flags)
{
	struct sctp_af *af;
	int err = -EINVAL;

	lock_sock(sk);
	pr_debug("%s: sk:%p, sockaddr:%p, addr_len:%d\n", __func__, sk,
		 addr, addr_len);

	/* Validate addr_len before calling common connect/connectx routine. */
	af = sctp_get_af_specific(addr->sa_family);
	if (af && addr_len >= af->sockaddr_len)
		err = __sctp_connect(sk, addr, af->sockaddr_len, flags, NULL);

	release_sock(sk);
	return err;
}