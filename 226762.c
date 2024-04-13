static int sctp_setsockopt_context(struct sock *sk,
				   struct sctp_assoc_value *params,
				   unsigned int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_association *asoc;

	if (optlen != sizeof(struct sctp_assoc_value))
		return -EINVAL;

	asoc = sctp_id2assoc(sk, params->assoc_id);
	if (!asoc && params->assoc_id > SCTP_ALL_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc) {
		asoc->default_rcv_context = params->assoc_value;

		return 0;
	}

	if (sctp_style(sk, TCP))
		params->assoc_id = SCTP_FUTURE_ASSOC;

	if (params->assoc_id == SCTP_FUTURE_ASSOC ||
	    params->assoc_id == SCTP_ALL_ASSOC)
		sp->default_rcv_context = params->assoc_value;

	if (params->assoc_id == SCTP_CURRENT_ASSOC ||
	    params->assoc_id == SCTP_ALL_ASSOC)
		list_for_each_entry(asoc, &sp->ep->asocs, asocs)
			asoc->default_rcv_context = params->assoc_value;

	return 0;
}