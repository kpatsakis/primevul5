static int sctp_setsockopt_pr_supported(struct sock *sk,
					struct sctp_assoc_value *params,
					unsigned int optlen)
{
	struct sctp_association *asoc;

	if (optlen != sizeof(*params))
		return -EINVAL;

	asoc = sctp_id2assoc(sk, params->assoc_id);
	if (!asoc && params->assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	sctp_sk(sk)->ep->prsctp_enable = !!params->assoc_value;

	return 0;
}