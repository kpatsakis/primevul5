static int sctp_setsockopt_event(struct sock *sk, struct sctp_event *param,
				 unsigned int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_association *asoc;
	int retval = 0;

	if (optlen < sizeof(*param))
		return -EINVAL;

	if (param->se_type < SCTP_SN_TYPE_BASE ||
	    param->se_type > SCTP_SN_TYPE_MAX)
		return -EINVAL;

	asoc = sctp_id2assoc(sk, param->se_assoc_id);
	if (!asoc && param->se_assoc_id > SCTP_ALL_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc)
		return sctp_assoc_ulpevent_type_set(param, asoc);

	if (sctp_style(sk, TCP))
		param->se_assoc_id = SCTP_FUTURE_ASSOC;

	if (param->se_assoc_id == SCTP_FUTURE_ASSOC ||
	    param->se_assoc_id == SCTP_ALL_ASSOC)
		sctp_ulpevent_type_set(&sp->subscribe,
				       param->se_type, param->se_on);

	if (param->se_assoc_id == SCTP_CURRENT_ASSOC ||
	    param->se_assoc_id == SCTP_ALL_ASSOC) {
		list_for_each_entry(asoc, &sp->ep->asocs, asocs) {
			int ret = sctp_assoc_ulpevent_type_set(param, asoc);

			if (ret && !retval)
				retval = ret;
		}
	}

	return retval;
}