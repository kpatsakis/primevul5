static int sctp_getsockopt_pf_expose(struct sock *sk, int len,
				     char __user *optval,
				     int __user *optlen)
{
	struct sctp_assoc_value params;
	struct sctp_association *asoc;
	int retval = -EFAULT;

	if (len < sizeof(params)) {
		retval = -EINVAL;
		goto out;
	}

	len = sizeof(params);
	if (copy_from_user(&params, optval, len))
		goto out;

	asoc = sctp_id2assoc(sk, params.assoc_id);
	if (!asoc && params.assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP)) {
		retval = -EINVAL;
		goto out;
	}

	params.assoc_value = asoc ? asoc->pf_expose
				  : sctp_sk(sk)->pf_expose;

	if (put_user(len, optlen))
		goto out;

	if (copy_to_user(optval, &params, len))
		goto out;

	retval = 0;

out:
	return retval;
}