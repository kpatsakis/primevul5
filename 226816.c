static int sctp_setsockopt_autoclose(struct sock *sk, u32 *optval,
				     unsigned int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct net *net = sock_net(sk);

	/* Applicable to UDP-style socket only */
	if (sctp_style(sk, TCP))
		return -EOPNOTSUPP;
	if (optlen != sizeof(int))
		return -EINVAL;

	sp->autoclose = *optval;
	if (sp->autoclose > net->sctp.max_autoclose)
		sp->autoclose = net->sctp.max_autoclose;

	return 0;
}