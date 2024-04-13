static int sctp_getsockopt_scheduler_value(struct sock *sk, int len,
					   char __user *optval,
					   int __user *optlen)
{
	struct sctp_stream_value params;
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
	if (!asoc) {
		retval = -EINVAL;
		goto out;
	}

	retval = sctp_sched_get_value(asoc, params.stream_id,
				      &params.stream_value);
	if (retval)
		goto out;

	if (put_user(len, optlen)) {
		retval = -EFAULT;
		goto out;
	}

	if (copy_to_user(optval, &params, len)) {
		retval = -EFAULT;
		goto out;
	}

out:
	return retval;
}