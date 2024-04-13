static int sctp_setsockopt_paddr_thresholds(struct sock *sk,
					    struct sctp_paddrthlds_v2 *val,
					    unsigned int optlen, bool v2)
{
	struct sctp_transport *trans;
	struct sctp_association *asoc;
	int len;

	len = v2 ? sizeof(*val) : sizeof(struct sctp_paddrthlds);
	if (optlen < len)
		return -EINVAL;

	if (v2 && val->spt_pathpfthld > val->spt_pathcpthld)
		return -EINVAL;

	if (!sctp_is_any(sk, (const union sctp_addr *)&val->spt_address)) {
		trans = sctp_addr_id2transport(sk, &val->spt_address,
					       val->spt_assoc_id);
		if (!trans)
			return -ENOENT;

		if (val->spt_pathmaxrxt)
			trans->pathmaxrxt = val->spt_pathmaxrxt;
		if (v2)
			trans->ps_retrans = val->spt_pathcpthld;
		trans->pf_retrans = val->spt_pathpfthld;

		return 0;
	}

	asoc = sctp_id2assoc(sk, val->spt_assoc_id);
	if (!asoc && val->spt_assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc) {
		list_for_each_entry(trans, &asoc->peer.transport_addr_list,
				    transports) {
			if (val->spt_pathmaxrxt)
				trans->pathmaxrxt = val->spt_pathmaxrxt;
			if (v2)
				trans->ps_retrans = val->spt_pathcpthld;
			trans->pf_retrans = val->spt_pathpfthld;
		}

		if (val->spt_pathmaxrxt)
			asoc->pathmaxrxt = val->spt_pathmaxrxt;
		if (v2)
			asoc->ps_retrans = val->spt_pathcpthld;
		asoc->pf_retrans = val->spt_pathpfthld;
	} else {
		struct sctp_sock *sp = sctp_sk(sk);

		if (val->spt_pathmaxrxt)
			sp->pathmaxrxt = val->spt_pathmaxrxt;
		if (v2)
			sp->ps_retrans = val->spt_pathcpthld;
		sp->pf_retrans = val->spt_pathpfthld;
	}

	return 0;
}