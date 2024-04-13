static int sctp_getsockopt_event(struct sock *sk, int len, char __user *optval,
				 int __user *optlen)
{
	struct sctp_association *asoc;
	struct sctp_event param;
	__u16 subscribe;

	if (len < sizeof(param))
		return -EINVAL;

	len = sizeof(param);
	if (copy_from_user(&param, optval, len))
		return -EFAULT;

	if (param.se_type < SCTP_SN_TYPE_BASE ||
	    param.se_type > SCTP_SN_TYPE_MAX)
		return -EINVAL;

	asoc = sctp_id2assoc(sk, param.se_assoc_id);
	if (!asoc && param.se_assoc_id != SCTP_FUTURE_ASSOC &&
	    sctp_style(sk, UDP))
		return -EINVAL;

	subscribe = asoc ? asoc->subscribe : sctp_sk(sk)->subscribe;
	param.se_on = sctp_ulpevent_type_enabled(subscribe, param.se_type);

	if (put_user(len, optlen))
		return -EFAULT;

	if (copy_to_user(optval, &param, len))
		return -EFAULT;

	return 0;
}