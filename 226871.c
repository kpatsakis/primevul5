static int sctp_setsockopt_nodelay(struct sock *sk, int *val,
				   unsigned int optlen)
{
	if (optlen < sizeof(int))
		return -EINVAL;
	sctp_sk(sk)->nodelay = (*val == 0) ? 0 : 1;
	return 0;
}