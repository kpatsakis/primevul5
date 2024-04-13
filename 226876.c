static int sctp_getsockopt_events(struct sock *sk, int len, char __user *optval,
				  int __user *optlen)
{
	struct sctp_event_subscribe subscribe;
	__u8 *sn_type = (__u8 *)&subscribe;
	int i;

	if (len == 0)
		return -EINVAL;
	if (len > sizeof(struct sctp_event_subscribe))
		len = sizeof(struct sctp_event_subscribe);
	if (put_user(len, optlen))
		return -EFAULT;

	for (i = 0; i < len; i++)
		sn_type[i] = sctp_ulpevent_type_enabled(sctp_sk(sk)->subscribe,
							SCTP_SN_TYPE_BASE + i);

	if (copy_to_user(optval, &subscribe, len))
		return -EFAULT;

	return 0;
}