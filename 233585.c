static int svc_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	int error, ep_ref;
	struct sockaddr_atmsvc sa;
	struct atm_vcc *vcc = ATM_SD(sock);

	switch (cmd) {
		case ATM_ADDPARTY:
			if (!test_bit(ATM_VF_SESSION, &vcc->flags))
				return -EINVAL;
			if (copy_from_user(&sa, (void __user *) arg, sizeof(sa)))
				return -EFAULT;
			error = svc_addparty(sock, (struct sockaddr *) &sa, sizeof(sa), 0);
			break;
		case ATM_DROPPARTY:
			if (!test_bit(ATM_VF_SESSION, &vcc->flags))
				return -EINVAL;
			if (copy_from_user(&ep_ref, (void __user *) arg, sizeof(int)))
				return -EFAULT;
			error = svc_dropparty(sock, ep_ref);
			break;
		default:
			error = vcc_ioctl(sock, cmd, arg);
	}

	return error;
}