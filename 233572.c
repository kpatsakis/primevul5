static int svc_getsockopt(struct socket *sock,int level,int optname,
    char __user *optval,int __user *optlen)
{
	struct sock *sk = sock->sk;
	int error = 0, len;

	lock_sock(sk);
	if (!__SO_LEVEL_MATCH(optname, level) || optname != SO_ATMSAP) {
		error = vcc_getsockopt(sock, level, optname, optval, optlen);
		goto out;
	}
	if (get_user(len, optlen)) {
		error = -EFAULT;
		goto out;
	}
	if (len != sizeof(struct atm_sap)) {
		error = -EINVAL;
		goto out;
	}
	if (copy_to_user(optval, &ATM_SD(sock)->sap, sizeof(struct atm_sap))) {
		error = -EFAULT;
		goto out;
	}
out:
	release_sock(sk);
	return error;
}