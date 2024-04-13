static int sctp_getsockopt_reuse_port(struct sock *sk, int len,
				      char __user *optval,
				      int __user *optlen)
{
	int val;

	if (len < sizeof(int))
		return -EINVAL;

	len = sizeof(int);
	val = sctp_sk(sk)->reuse;
	if (put_user(len, optlen))
		return -EFAULT;

	if (copy_to_user(optval, &val, len))
		return -EFAULT;

	return 0;
}