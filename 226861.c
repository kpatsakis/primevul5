static int sctp_setsockopt_enable_strreset(struct sock *sk,
					   struct sctp_assoc_value *params,
					   unsigned int optlen)
{
	struct sctp_endpoint *ep = sctp_sk(sk)->ep;
	struct sctp_association *asoc;
	int retval = -EINVAL;

	if (optlen != sizeof(*params))
		goto out;

	if (params->assoc_value & (~SCTP_ENABLE_STRRESET_MASK))
		goto out;

	asoc = sctp_id2assoc(sk, params->assoc_id);
	if (!asoc && params->assoc_id > SCTP_ALL_ASSOC &&
	    sctp_style(sk, UDP))
		goto out;

	retval = 0;

	if (asoc) {
		asoc->strreset_enable = params->assoc_value;
		goto out;
	}

	if (sctp_style(sk, TCP))
		params->assoc_id = SCTP_FUTURE_ASSOC;

	if (params->assoc_id == SCTP_FUTURE_ASSOC ||
	    params->assoc_id == SCTP_ALL_ASSOC)
		ep->strreset_enable = params->assoc_value;

	if (params->assoc_id == SCTP_CURRENT_ASSOC ||
	    params->assoc_id == SCTP_ALL_ASSOC)
		list_for_each_entry(asoc, &ep->asocs, asocs)
			asoc->strreset_enable = params->assoc_value;

out:
	return retval;
}