static int sctp_setsockopt_scheduler(struct sock *sk,
				     struct sctp_assoc_value *params,
				     unsigned int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_association *asoc;
	int retval = 0;

	if (optlen < sizeof(*params))
		return -EINVAL;

	if (params->assoc_value > SCTP_SS_MAX)
		return -EINVAL;

	asoc = sctp_id2assoc(sk, params->assoc_id);
	if (!asoc && params->assoc_id > SCTP_ALL_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc)
		return sctp_sched_set_sched(asoc, params->assoc_value);

	if (sctp_style(sk, TCP))
		params->assoc_id = SCTP_FUTURE_ASSOC;

	if (params->assoc_id == SCTP_FUTURE_ASSOC ||
	    params->assoc_id == SCTP_ALL_ASSOC)
		sp->default_ss = params->assoc_value;

	if (params->assoc_id == SCTP_CURRENT_ASSOC ||
	    params->assoc_id == SCTP_ALL_ASSOC) {
		list_for_each_entry(asoc, &sp->ep->asocs, asocs) {
			int ret = sctp_sched_set_sched(asoc,
						       params->assoc_value);

			if (ret && !retval)
				retval = ret;
		}
	}

	return retval;
}