static int sctp_getsockopt_pr_streamstatus(struct sock *sk, int len,
					   char __user *optval,
					   int __user *optlen)
{
	struct sctp_stream_out_ext *streamoute;
	struct sctp_association *asoc;
	struct sctp_prstatus params;
	int retval = -EINVAL;
	int policy;

	if (len < sizeof(params))
		goto out;

	len = sizeof(params);
	if (copy_from_user(&params, optval, len)) {
		retval = -EFAULT;
		goto out;
	}

	policy = params.sprstat_policy;
	if (!policy || (policy & ~(SCTP_PR_SCTP_MASK | SCTP_PR_SCTP_ALL)) ||
	    ((policy & SCTP_PR_SCTP_ALL) && (policy & SCTP_PR_SCTP_MASK)))
		goto out;

	asoc = sctp_id2assoc(sk, params.sprstat_assoc_id);
	if (!asoc || params.sprstat_sid >= asoc->stream.outcnt)
		goto out;

	streamoute = SCTP_SO(&asoc->stream, params.sprstat_sid)->ext;
	if (!streamoute) {
		/* Not allocated yet, means all stats are 0 */
		params.sprstat_abandoned_unsent = 0;
		params.sprstat_abandoned_sent = 0;
		retval = 0;
		goto out;
	}

	if (policy == SCTP_PR_SCTP_ALL) {
		params.sprstat_abandoned_unsent = 0;
		params.sprstat_abandoned_sent = 0;
		for (policy = 0; policy <= SCTP_PR_INDEX(MAX); policy++) {
			params.sprstat_abandoned_unsent +=
				streamoute->abandoned_unsent[policy];
			params.sprstat_abandoned_sent +=
				streamoute->abandoned_sent[policy];
		}
	} else {
		params.sprstat_abandoned_unsent =
			streamoute->abandoned_unsent[__SCTP_PR_INDEX(policy)];
		params.sprstat_abandoned_sent =
			streamoute->abandoned_sent[__SCTP_PR_INDEX(policy)];
	}

	if (put_user(len, optlen) || copy_to_user(optval, &params, len)) {
		retval = -EFAULT;
		goto out;
	}

	retval = 0;

out:
	return retval;
}