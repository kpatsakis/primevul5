static int sctp_setsockopt_maxseg(struct sock *sk,
				  struct sctp_assoc_value *params,
				  unsigned int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_association *asoc;
	sctp_assoc_t assoc_id;
	int val;

	if (optlen == sizeof(int)) {
		pr_warn_ratelimited(DEPRECATED
				    "%s (pid %d) "
				    "Use of int in maxseg socket option.\n"
				    "Use struct sctp_assoc_value instead\n",
				    current->comm, task_pid_nr(current));
		assoc_id = SCTP_FUTURE_ASSOC;
		val = *(int *)params;
	} else if (optlen == sizeof(struct sctp_assoc_value)) {
		assoc_id = params->assoc_id;
		val = params->assoc_value;
	} else {
		return -EINVAL;
	}

	asoc = sctp_id2assoc(sk, assoc_id);
	if (!asoc && assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	if (val) {
		int min_len, max_len;
		__u16 datasize = asoc ? sctp_datachk_len(&asoc->stream) :
				 sizeof(struct sctp_data_chunk);

		min_len = sctp_min_frag_point(sp, datasize);
		max_len = SCTP_MAX_CHUNK_LEN - datasize;

		if (val < min_len || val > max_len)
			return -EINVAL;
	}

	if (asoc) {
		asoc->user_frag = val;
		sctp_assoc_update_frag_point(asoc);
	} else {
		sp->user_frag = val;
	}

	return 0;
}