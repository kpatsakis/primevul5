static int sctp_setsockopt_reuse_port(struct sock *sk, int *val,
				      unsigned int optlen)
{
	if (!sctp_style(sk, TCP))
		return -EOPNOTSUPP;

	if (sctp_sk(sk)->ep->base.bind_addr.port)
		return -EFAULT;

	if (optlen < sizeof(int))
		return -EINVAL;

	sctp_sk(sk)->reuse = !!*val;

	return 0;
}