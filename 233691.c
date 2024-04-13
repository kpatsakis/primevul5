static int irda_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	struct sock *sk = sock->sk;

	IRDA_DEBUG(4, "%s(), cmd=%#x\n", __func__, cmd);

	switch (cmd) {
	case TIOCOUTQ: {
		long amount;

		amount = sk->sk_sndbuf - sk_wmem_alloc_get(sk);
		if (amount < 0)
			amount = 0;
		if (put_user(amount, (unsigned int __user *)arg))
			return -EFAULT;
		return 0;
	}

	case TIOCINQ: {
		struct sk_buff *skb;
		long amount = 0L;
		/* These two are safe on a single CPU system as only user tasks fiddle here */
		if ((skb = skb_peek(&sk->sk_receive_queue)) != NULL)
			amount = skb->len;
		if (put_user(amount, (unsigned int __user *)arg))
			return -EFAULT;
		return 0;
	}

	case SIOCGSTAMP:
		if (sk != NULL)
			return sock_get_timestamp(sk, (struct timeval __user *)arg);
		return -EINVAL;

	case SIOCGIFADDR:
	case SIOCSIFADDR:
	case SIOCGIFDSTADDR:
	case SIOCSIFDSTADDR:
	case SIOCGIFBRDADDR:
	case SIOCSIFBRDADDR:
	case SIOCGIFNETMASK:
	case SIOCSIFNETMASK:
	case SIOCGIFMETRIC:
	case SIOCSIFMETRIC:
		return -EINVAL;
	default:
		IRDA_DEBUG(1, "%s(), doing device ioctl!\n", __func__);
		return -ENOIOCTLCMD;
	}

	/*NOTREACHED*/
	return 0;
}