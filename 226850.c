static int sctp_getsockopt_context(struct sock *sk, int len,
				   char __user *optval, int __user *optlen)
{
	struct sctp_assoc_value params;
	struct sctp_association *asoc;

	if (len < sizeof(struct sctp_assoc_value))
		return -EINVAL;

	len = sizeof(struct sctp_assoc_value);

	if (copy_from_user(&params, optval, len))
		return -EFAULT;

	asoc = sctp_id2assoc(sk, params.assoc_id);
	if (!asoc && params.assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	params.assoc_value = asoc ? asoc->default_rcv_context
				  : sctp_sk(sk)->default_rcv_context;

	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, &params, len))
		return -EFAULT;

	return 0;
}