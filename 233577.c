static int svc_setsockopt(struct socket *sock, int level, int optname,
			  char __user *optval, int optlen)
{
	struct sock *sk = sock->sk;
	struct atm_vcc *vcc = ATM_SD(sock);
	int value, error = 0;

	lock_sock(sk);
	switch (optname) {
		case SO_ATMSAP:
			if (level != SOL_ATM || optlen != sizeof(struct atm_sap)) {
				error = -EINVAL;
				goto out;
			}
			if (copy_from_user(&vcc->sap, optval, optlen)) {
				error = -EFAULT;
				goto out;
			}
			set_bit(ATM_VF_HASSAP, &vcc->flags);
			break;
		case SO_MULTIPOINT:
			if (level != SOL_ATM || optlen != sizeof(int)) {
				error = -EINVAL;
				goto out;
			}
			if (get_user(value, (int __user *) optval)) {
				error = -EFAULT;
				goto out;
			}
			if (value == 1) {
				set_bit(ATM_VF_SESSION, &vcc->flags);
			} else if (value == 0) {
				clear_bit(ATM_VF_SESSION, &vcc->flags);
			} else {
				error = -EINVAL;
			}
			break;
		default:
			error = vcc_setsockopt(sock, level, optname,
					       optval, optlen);
	}

out:
	release_sock(sk);
	return error;
}