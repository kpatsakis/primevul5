static int sctp_setsockopt_scheduler_value(struct sock *sk,
					   struct sctp_stream_value *params,
					   unsigned int optlen)
{
	struct sctp_association *asoc;
	int retval = -EINVAL;

	if (optlen < sizeof(*params))
		goto out;

	asoc = sctp_id2assoc(sk, params->assoc_id);
	if (!asoc && params->assoc_id != SCTP_CURRENT_ASSOC &&
	    sctp_style(sk, UDP))
		goto out;

	if (asoc) {
		retval = sctp_sched_set_value(asoc, params->stream_id,
					      params->stream_value, GFP_KERNEL);
		goto out;
	}

	retval = 0;

	list_for_each_entry(asoc, &sctp_sk(sk)->ep->asocs, asocs) {
		int ret = sctp_sched_set_value(asoc, params->stream_id,
					       params->stream_value,
					       GFP_KERNEL);
		if (ret && !retval) /* try to return the 1st error. */
			retval = ret;
	}

out:
	return retval;
}