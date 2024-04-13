static int sctp_setsockopt_disable_fragments(struct sock *sk, int *val,
					     unsigned int optlen)
{
	if (optlen < sizeof(int))
		return -EINVAL;
	sctp_sk(sk)->disable_fragments = (*val == 0) ? 0 : 1;
	return 0;
}