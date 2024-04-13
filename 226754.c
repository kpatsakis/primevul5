static int sctp_setsockopt_mappedv4(struct sock *sk, int *val,
				    unsigned int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);

	if (optlen < sizeof(int))
		return -EINVAL;
	if (*val)
		sp->v4mapped = 1;
	else
		sp->v4mapped = 0;

	return 0;
}