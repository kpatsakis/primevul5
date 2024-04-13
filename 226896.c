static int sctp_getsockopt_recvrcvinfo(struct sock *sk,	int len,
				       char __user *optval,
				       int __user *optlen)
{
	int val = 0;

	if (len < sizeof(int))
		return -EINVAL;

	len = sizeof(int);
	if (sctp_sk(sk)->recvrcvinfo)
		val = 1;
	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, &val, len))
		return -EFAULT;

	return 0;
}