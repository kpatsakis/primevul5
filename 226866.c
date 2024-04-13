static int sctp_setsockopt_default_send_param(struct sock *sk,
					      struct sctp_sndrcvinfo *info,
					      unsigned int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_association *asoc;

	if (optlen != sizeof(*info))
		return -EINVAL;
	if (info->sinfo_flags &
	    ~(SCTP_UNORDERED | SCTP_ADDR_OVER |
	      SCTP_ABORT | SCTP_EOF))
		return -EINVAL;

	asoc = sctp_id2assoc(sk, info->sinfo_assoc_id);
	if (!asoc && info->sinfo_assoc_id > SCTP_ALL_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc) {
		asoc->default_stream = info->sinfo_stream;
		asoc->default_flags = info->sinfo_flags;
		asoc->default_ppid = info->sinfo_ppid;
		asoc->default_context = info->sinfo_context;
		asoc->default_timetolive = info->sinfo_timetolive;

		return 0;
	}

	if (sctp_style(sk, TCP))
		info->sinfo_assoc_id = SCTP_FUTURE_ASSOC;

	if (info->sinfo_assoc_id == SCTP_FUTURE_ASSOC ||
	    info->sinfo_assoc_id == SCTP_ALL_ASSOC) {
		sp->default_stream = info->sinfo_stream;
		sp->default_flags = info->sinfo_flags;
		sp->default_ppid = info->sinfo_ppid;
		sp->default_context = info->sinfo_context;
		sp->default_timetolive = info->sinfo_timetolive;
	}

	if (info->sinfo_assoc_id == SCTP_CURRENT_ASSOC ||
	    info->sinfo_assoc_id == SCTP_ALL_ASSOC) {
		list_for_each_entry(asoc, &sp->ep->asocs, asocs) {
			asoc->default_stream = info->sinfo_stream;
			asoc->default_flags = info->sinfo_flags;
			asoc->default_ppid = info->sinfo_ppid;
			asoc->default_context = info->sinfo_context;
			asoc->default_timetolive = info->sinfo_timetolive;
		}
	}

	return 0;
}