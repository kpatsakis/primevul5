static int sctp_sendmsg_parse(struct sock *sk, struct sctp_cmsgs *cmsgs,
			      struct sctp_sndrcvinfo *srinfo,
			      const struct msghdr *msg, size_t msg_len)
{
	__u16 sflags;
	int err;

	if (sctp_sstate(sk, LISTENING) && sctp_style(sk, TCP))
		return -EPIPE;

	if (msg_len > sk->sk_sndbuf)
		return -EMSGSIZE;

	memset(cmsgs, 0, sizeof(*cmsgs));
	err = sctp_msghdr_parse(msg, cmsgs);
	if (err) {
		pr_debug("%s: msghdr parse err:%x\n", __func__, err);
		return err;
	}

	memset(srinfo, 0, sizeof(*srinfo));
	if (cmsgs->srinfo) {
		srinfo->sinfo_stream = cmsgs->srinfo->sinfo_stream;
		srinfo->sinfo_flags = cmsgs->srinfo->sinfo_flags;
		srinfo->sinfo_ppid = cmsgs->srinfo->sinfo_ppid;
		srinfo->sinfo_context = cmsgs->srinfo->sinfo_context;
		srinfo->sinfo_assoc_id = cmsgs->srinfo->sinfo_assoc_id;
		srinfo->sinfo_timetolive = cmsgs->srinfo->sinfo_timetolive;
	}

	if (cmsgs->sinfo) {
		srinfo->sinfo_stream = cmsgs->sinfo->snd_sid;
		srinfo->sinfo_flags = cmsgs->sinfo->snd_flags;
		srinfo->sinfo_ppid = cmsgs->sinfo->snd_ppid;
		srinfo->sinfo_context = cmsgs->sinfo->snd_context;
		srinfo->sinfo_assoc_id = cmsgs->sinfo->snd_assoc_id;
	}

	if (cmsgs->prinfo) {
		srinfo->sinfo_timetolive = cmsgs->prinfo->pr_value;
		SCTP_PR_SET_POLICY(srinfo->sinfo_flags,
				   cmsgs->prinfo->pr_policy);
	}

	sflags = srinfo->sinfo_flags;
	if (!sflags && msg_len)
		return 0;

	if (sctp_style(sk, TCP) && (sflags & (SCTP_EOF | SCTP_ABORT)))
		return -EINVAL;

	if (((sflags & SCTP_EOF) && msg_len > 0) ||
	    (!(sflags & (SCTP_EOF | SCTP_ABORT)) && msg_len == 0))
		return -EINVAL;

	if ((sflags & SCTP_ADDR_OVER) && !msg->msg_name)
		return -EINVAL;

	return 0;
}