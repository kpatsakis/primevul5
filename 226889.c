static int sctp_getsockopt_paddr_thresholds(struct sock *sk,
					    char __user *optval, int len,
					    int __user *optlen, bool v2)
{
	struct sctp_paddrthlds_v2 val;
	struct sctp_transport *trans;
	struct sctp_association *asoc;
	int min;

	min = v2 ? sizeof(val) : sizeof(struct sctp_paddrthlds);
	if (len < min)
		return -EINVAL;
	len = min;
	if (copy_from_user(&val, optval, len))
		return -EFAULT;

	if (!sctp_is_any(sk, (const union sctp_addr *)&val.spt_address)) {
		trans = sctp_addr_id2transport(sk, &val.spt_address,
					       val.spt_assoc_id);
		if (!trans)
			return -ENOENT;

		val.spt_pathmaxrxt = trans->pathmaxrxt;
		val.spt_pathpfthld = trans->pf_retrans;
		val.spt_pathcpthld = trans->ps_retrans;

		goto out;
	}

	asoc = sctp_id2assoc(sk, val.spt_assoc_id);
	if (!asoc && val.spt_assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc) {
		val.spt_pathpfthld = asoc->pf_retrans;
		val.spt_pathmaxrxt = asoc->pathmaxrxt;
		val.spt_pathcpthld = asoc->ps_retrans;
	} else {
		struct sctp_sock *sp = sctp_sk(sk);

		val.spt_pathpfthld = sp->pf_retrans;
		val.spt_pathmaxrxt = sp->pathmaxrxt;
		val.spt_pathcpthld = sp->ps_retrans;
	}

out:
	if (put_user(len, optlen) || copy_to_user(optval, &val, len))
		return -EFAULT;

	return 0;
}