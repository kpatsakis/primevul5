static int sctp_setsockopt_reconfig_supported(struct sock *sk,
					      struct sctp_assoc_value *params,
					      unsigned int optlen)
{
	struct sctp_association *asoc;
	int retval = -EINVAL;

	if (optlen != sizeof(*params))
		goto out;

	asoc = sctp_id2assoc(sk, params->assoc_id);
	if (!asoc && params->assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP))
		goto out;

	sctp_sk(sk)->ep->reconf_enable = !!params->assoc_value;

	retval = 0;

out:
	return retval;
}