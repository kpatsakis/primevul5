static int __sctp_setsockopt_delayed_ack(struct sock *sk,
					 struct sctp_sack_info *params)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_association *asoc;

	/* Validate value parameter. */
	if (params->sack_delay > 500)
		return -EINVAL;

	/* Get association, if sack_assoc_id != SCTP_FUTURE_ASSOC and the
	 * socket is a one to many style socket, and an association
	 * was not found, then the id was invalid.
	 */
	asoc = sctp_id2assoc(sk, params->sack_assoc_id);
	if (!asoc && params->sack_assoc_id > SCTP_ALL_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc) {
		sctp_apply_asoc_delayed_ack(params, asoc);

		return 0;
	}

	if (sctp_style(sk, TCP))
		params->sack_assoc_id = SCTP_FUTURE_ASSOC;

	if (params->sack_assoc_id == SCTP_FUTURE_ASSOC ||
	    params->sack_assoc_id == SCTP_ALL_ASSOC) {
		if (params->sack_delay) {
			sp->sackdelay = params->sack_delay;
			sp->param_flags =
				sctp_spp_sackdelay_enable(sp->param_flags);
		}
		if (params->sack_freq == 1) {
			sp->param_flags =
				sctp_spp_sackdelay_disable(sp->param_flags);
		} else if (params->sack_freq > 1) {
			sp->sackfreq = params->sack_freq;
			sp->param_flags =
				sctp_spp_sackdelay_enable(sp->param_flags);
		}
	}

	if (params->sack_assoc_id == SCTP_CURRENT_ASSOC ||
	    params->sack_assoc_id == SCTP_ALL_ASSOC)
		list_for_each_entry(asoc, &sp->ep->asocs, asocs)
			sctp_apply_asoc_delayed_ack(params, asoc);

	return 0;
}