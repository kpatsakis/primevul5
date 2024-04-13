static int sctp_getsockopt_delayed_ack(struct sock *sk, int len,
					    char __user *optval,
					    int __user *optlen)
{
	struct sctp_sack_info    params;
	struct sctp_association *asoc = NULL;
	struct sctp_sock        *sp = sctp_sk(sk);

	if (len >= sizeof(struct sctp_sack_info)) {
		len = sizeof(struct sctp_sack_info);

		if (copy_from_user(&params, optval, len))
			return -EFAULT;
	} else if (len == sizeof(struct sctp_assoc_value)) {
		pr_warn_ratelimited(DEPRECATED
				    "%s (pid %d) "
				    "Use of struct sctp_assoc_value in delayed_ack socket option.\n"
				    "Use struct sctp_sack_info instead\n",
				    current->comm, task_pid_nr(current));
		if (copy_from_user(&params, optval, len))
			return -EFAULT;
	} else
		return -EINVAL;

	/* Get association, if sack_assoc_id != SCTP_FUTURE_ASSOC and the
	 * socket is a one to many style socket, and an association
	 * was not found, then the id was invalid.
	 */
	asoc = sctp_id2assoc(sk, params.sack_assoc_id);
	if (!asoc && params.sack_assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc) {
		/* Fetch association values. */
		if (asoc->param_flags & SPP_SACKDELAY_ENABLE) {
			params.sack_delay = jiffies_to_msecs(asoc->sackdelay);
			params.sack_freq = asoc->sackfreq;

		} else {
			params.sack_delay = 0;
			params.sack_freq = 1;
		}
	} else {
		/* Fetch socket values. */
		if (sp->param_flags & SPP_SACKDELAY_ENABLE) {
			params.sack_delay  = sp->sackdelay;
			params.sack_freq = sp->sackfreq;
		} else {
			params.sack_delay  = 0;
			params.sack_freq = 1;
		}
	}

	if (copy_to_user(optval, &params, len))
		return -EFAULT;

	if (put_user(len, optlen))
		return -EFAULT;

	return 0;
}