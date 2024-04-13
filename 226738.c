static int sctp_setsockopt_pf_expose(struct sock *sk,
				     struct sctp_assoc_value *params,
				     unsigned int optlen)
{
	struct sctp_association *asoc;
	int retval = -EINVAL;

	if (optlen != sizeof(*params))
		goto out;

	if (params->assoc_value > SCTP_PF_EXPOSE_MAX)
		goto out;

	asoc = sctp_id2assoc(sk, params->assoc_id);
	if (!asoc && params->assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP))
		goto out;

	if (asoc)
		asoc->pf_expose = params->assoc_value;
	else
		sctp_sk(sk)->pf_expose = params->assoc_value;
	retval = 0;

out:
	return retval;
}