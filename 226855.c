static int sctp_setsockopt_interleaving_supported(struct sock *sk,
						  struct sctp_assoc_value *p,
						  unsigned int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_association *asoc;

	if (optlen < sizeof(*p))
		return -EINVAL;

	asoc = sctp_id2assoc(sk, p->assoc_id);
	if (!asoc && p->assoc_id != SCTP_FUTURE_ASSOC && sctp_style(sk, UDP))
		return -EINVAL;

	if (!sock_net(sk)->sctp.intl_enable || !sp->frag_interleave) {
		return -EPERM;
	}

	sp->ep->intl_enable = !!p->assoc_value;
	return 0;
}