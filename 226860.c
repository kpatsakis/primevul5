static int sctp_setsockopt_fragment_interleave(struct sock *sk, int *val,
					       unsigned int optlen)
{
	if (optlen != sizeof(int))
		return -EINVAL;

	sctp_sk(sk)->frag_interleave = !!*val;

	if (!sctp_sk(sk)->frag_interleave)
		sctp_sk(sk)->ep->intl_enable = 0;

	return 0;
}