static int sctp_setsockopt_delayed_ack(struct sock *sk,
				       struct sctp_sack_info *params,
				       unsigned int optlen)
{
	if (optlen == sizeof(struct sctp_assoc_value)) {
		struct sctp_assoc_value *v = (struct sctp_assoc_value *)params;
		struct sctp_sack_info p;

		pr_warn_ratelimited(DEPRECATED
				    "%s (pid %d) "
				    "Use of struct sctp_assoc_value in delayed_ack socket option.\n"
				    "Use struct sctp_sack_info instead\n",
				    current->comm, task_pid_nr(current));

		p.sack_assoc_id = v->assoc_id;
		p.sack_delay = v->assoc_value;
		p.sack_freq = v->assoc_value ? 0 : 1;
		return __sctp_setsockopt_delayed_ack(sk, &p);
	}

	if (optlen != sizeof(struct sctp_sack_info))
		return -EINVAL;
	if (params->sack_delay == 0 && params->sack_freq == 0)
		return 0;
	return __sctp_setsockopt_delayed_ack(sk, params);
}