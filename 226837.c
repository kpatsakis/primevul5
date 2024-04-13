static int sctp_setsockopt_maxburst(struct sock *sk,
				    struct sctp_assoc_value *params,
				    unsigned int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_association *asoc;
	sctp_assoc_t assoc_id;
	u32 assoc_value;

	if (optlen == sizeof(int)) {
		pr_warn_ratelimited(DEPRECATED
				    "%s (pid %d) "
				    "Use of int in max_burst socket option deprecated.\n"
				    "Use struct sctp_assoc_value instead\n",
				    current->comm, task_pid_nr(current));
		assoc_id = SCTP_FUTURE_ASSOC;
		assoc_value = *((int *)params);
	} else if (optlen == sizeof(struct sctp_assoc_value)) {
		assoc_id = params->assoc_id;
		assoc_value = params->assoc_value;
	} else
		return -EINVAL;

	asoc = sctp_id2assoc(sk, assoc_id);
	if (!asoc && assoc_id > SCTP_ALL_ASSOC && sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc) {
		asoc->max_burst = assoc_value;

		return 0;
	}

	if (sctp_style(sk, TCP))
		assoc_id = SCTP_FUTURE_ASSOC;

	if (assoc_id == SCTP_FUTURE_ASSOC || assoc_id == SCTP_ALL_ASSOC)
		sp->max_burst = assoc_value;

	if (assoc_id == SCTP_CURRENT_ASSOC || assoc_id == SCTP_ALL_ASSOC)
		list_for_each_entry(asoc, &sp->ep->asocs, asocs)
			asoc->max_burst = assoc_value;

	return 0;
}