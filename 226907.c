static int sctp_getsockopt_default_sndinfo(struct sock *sk, int len,
					   char __user *optval,
					   int __user *optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_association *asoc;
	struct sctp_sndinfo info;

	if (len < sizeof(info))
		return -EINVAL;

	len = sizeof(info);

	if (copy_from_user(&info, optval, len))
		return -EFAULT;

	asoc = sctp_id2assoc(sk, info.snd_assoc_id);
	if (!asoc && info.snd_assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc) {
		info.snd_sid = asoc->default_stream;
		info.snd_flags = asoc->default_flags;
		info.snd_ppid = asoc->default_ppid;
		info.snd_context = asoc->default_context;
	} else {
		info.snd_sid = sp->default_stream;
		info.snd_flags = sp->default_flags;
		info.snd_ppid = sp->default_ppid;
		info.snd_context = sp->default_context;
	}

	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, &info, len))
		return -EFAULT;

	return 0;
}