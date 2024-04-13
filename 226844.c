static int sctp_setsockopt_default_prinfo(struct sock *sk,
					  struct sctp_default_prinfo *info,
					  unsigned int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_association *asoc;
	int retval = -EINVAL;

	if (optlen != sizeof(*info))
		goto out;

	if (info->pr_policy & ~SCTP_PR_SCTP_MASK)
		goto out;

	if (info->pr_policy == SCTP_PR_SCTP_NONE)
		info->pr_value = 0;

	asoc = sctp_id2assoc(sk, info->pr_assoc_id);
	if (!asoc && info->pr_assoc_id > SCTP_ALL_ASSOC &&
	    sctp_style(sk, UDP))
		goto out;

	retval = 0;

	if (asoc) {
		SCTP_PR_SET_POLICY(asoc->default_flags, info->pr_policy);
		asoc->default_timetolive = info->pr_value;
		goto out;
	}

	if (sctp_style(sk, TCP))
		info->pr_assoc_id = SCTP_FUTURE_ASSOC;

	if (info->pr_assoc_id == SCTP_FUTURE_ASSOC ||
	    info->pr_assoc_id == SCTP_ALL_ASSOC) {
		SCTP_PR_SET_POLICY(sp->default_flags, info->pr_policy);
		sp->default_timetolive = info->pr_value;
	}

	if (info->pr_assoc_id == SCTP_CURRENT_ASSOC ||
	    info->pr_assoc_id == SCTP_ALL_ASSOC) {
		list_for_each_entry(asoc, &sp->ep->asocs, asocs) {
			SCTP_PR_SET_POLICY(asoc->default_flags,
					   info->pr_policy);
			asoc->default_timetolive = info->pr_value;
		}
	}

out:
	return retval;
}