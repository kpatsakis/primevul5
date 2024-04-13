static int sctp_setsockopt_default_sndinfo(struct sock *sk,
					   struct sctp_sndinfo *info,
					   unsigned int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_association *asoc;

	if (optlen != sizeof(*info))
		return -EINVAL;
	if (info->snd_flags &
	    ~(SCTP_UNORDERED | SCTP_ADDR_OVER |
	      SCTP_ABORT | SCTP_EOF))
		return -EINVAL;

	asoc = sctp_id2assoc(sk, info->snd_assoc_id);
	if (!asoc && info->snd_assoc_id > SCTP_ALL_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc) {
		asoc->default_stream = info->snd_sid;
		asoc->default_flags = info->snd_flags;
		asoc->default_ppid = info->snd_ppid;
		asoc->default_context = info->snd_context;

		return 0;
	}

	if (sctp_style(sk, TCP))
		info->snd_assoc_id = SCTP_FUTURE_ASSOC;

	if (info->snd_assoc_id == SCTP_FUTURE_ASSOC ||
	    info->snd_assoc_id == SCTP_ALL_ASSOC) {
		sp->default_stream = info->snd_sid;
		sp->default_flags = info->snd_flags;
		sp->default_ppid = info->snd_ppid;
		sp->default_context = info->snd_context;
	}

	if (info->snd_assoc_id == SCTP_CURRENT_ASSOC ||
	    info->snd_assoc_id == SCTP_ALL_ASSOC) {
		list_for_each_entry(asoc, &sp->ep->asocs, asocs) {
			asoc->default_stream = info->snd_sid;
			asoc->default_flags = info->snd_flags;
			asoc->default_ppid = info->snd_ppid;
			asoc->default_context = info->snd_context;
		}
	}

	return 0;
}