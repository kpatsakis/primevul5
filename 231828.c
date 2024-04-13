static int x25_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	struct sock *sk = sock->sk;
	struct x25_sock *x25 = x25_sk(sk);
	void __user *argp = (void __user *)arg;
	int rc;

	switch (cmd) {
	case TIOCOUTQ: {
		int amount;

		amount = sk->sk_sndbuf - sk_wmem_alloc_get(sk);
		if (amount < 0)
			amount = 0;
		rc = put_user(amount, (unsigned int __user *)argp);
		break;
	}

	case TIOCINQ: {
		struct sk_buff *skb;
		int amount = 0;
		/*
		 * These two are safe on a single CPU system as
		 * only user tasks fiddle here
		 */
		lock_sock(sk);
		if ((skb = skb_peek(&sk->sk_receive_queue)) != NULL)
			amount = skb->len;
		release_sock(sk);
		rc = put_user(amount, (unsigned int __user *)argp);
		break;
	}

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
		rc = -EINVAL;
		break;
	case SIOCADDRT:
	case SIOCDELRT:
		rc = -EPERM;
		if (!capable(CAP_NET_ADMIN))
			break;
		rc = x25_route_ioctl(cmd, argp);
		break;
	case SIOCX25GSUBSCRIP:
		rc = x25_subscr_ioctl(cmd, argp);
		break;
	case SIOCX25SSUBSCRIP:
		rc = -EPERM;
		if (!capable(CAP_NET_ADMIN))
			break;
		rc = x25_subscr_ioctl(cmd, argp);
		break;
	case SIOCX25GFACILITIES: {
		lock_sock(sk);
		rc = copy_to_user(argp, &x25->facilities,
				  sizeof(x25->facilities))
			? -EFAULT : 0;
		release_sock(sk);
		break;
	}

	case SIOCX25SFACILITIES: {
		struct x25_facilities facilities;
		rc = -EFAULT;
		if (copy_from_user(&facilities, argp, sizeof(facilities)))
			break;
		rc = -EINVAL;
		lock_sock(sk);
		if (sk->sk_state != TCP_LISTEN &&
		    sk->sk_state != TCP_CLOSE)
			goto out_fac_release;
		if (facilities.pacsize_in < X25_PS16 ||
		    facilities.pacsize_in > X25_PS4096)
			goto out_fac_release;
		if (facilities.pacsize_out < X25_PS16 ||
		    facilities.pacsize_out > X25_PS4096)
			goto out_fac_release;
		if (facilities.winsize_in < 1 ||
		    facilities.winsize_in > 127)
			goto out_fac_release;
		if (facilities.throughput) {
			int out = facilities.throughput & 0xf0;
			int in  = facilities.throughput & 0x0f;
			if (!out)
				facilities.throughput |=
					X25_DEFAULT_THROUGHPUT << 4;
			else if (out < 0x30 || out > 0xD0)
				goto out_fac_release;
			if (!in)
				facilities.throughput |=
					X25_DEFAULT_THROUGHPUT;
			else if (in < 0x03 || in > 0x0D)
				goto out_fac_release;
		}
		if (facilities.reverse &&
		    (facilities.reverse & 0x81) != 0x81)
			goto out_fac_release;
		x25->facilities = facilities;
		rc = 0;
out_fac_release:
		release_sock(sk);
		break;
	}

	case SIOCX25GDTEFACILITIES: {
		lock_sock(sk);
		rc = copy_to_user(argp, &x25->dte_facilities,
				  sizeof(x25->dte_facilities));
		release_sock(sk);
		if (rc)
			rc = -EFAULT;
		break;
	}

	case SIOCX25SDTEFACILITIES: {
		struct x25_dte_facilities dtefacs;
		rc = -EFAULT;
		if (copy_from_user(&dtefacs, argp, sizeof(dtefacs)))
			break;
		rc = -EINVAL;
		lock_sock(sk);
		if (sk->sk_state != TCP_LISTEN &&
		    sk->sk_state != TCP_CLOSE)
			goto out_dtefac_release;
		if (dtefacs.calling_len > X25_MAX_AE_LEN)
			goto out_dtefac_release;
		if (dtefacs.called_len > X25_MAX_AE_LEN)
			goto out_dtefac_release;
		x25->dte_facilities = dtefacs;
		rc = 0;
out_dtefac_release:
		release_sock(sk);
		break;
	}

	case SIOCX25GCALLUSERDATA: {
		lock_sock(sk);
		rc = copy_to_user(argp, &x25->calluserdata,
				  sizeof(x25->calluserdata))
			? -EFAULT : 0;
		release_sock(sk);
		break;
	}

	case SIOCX25SCALLUSERDATA: {
		struct x25_calluserdata calluserdata;

		rc = -EFAULT;
		if (copy_from_user(&calluserdata, argp, sizeof(calluserdata)))
			break;
		rc = -EINVAL;
		if (calluserdata.cudlength > X25_MAX_CUD_LEN)
			break;
		lock_sock(sk);
		x25->calluserdata = calluserdata;
		release_sock(sk);
		rc = 0;
		break;
	}

	case SIOCX25GCAUSEDIAG: {
		lock_sock(sk);
		rc = copy_to_user(argp, &x25->causediag, sizeof(x25->causediag))
			? -EFAULT : 0;
		release_sock(sk);
		break;
	}

	case SIOCX25SCAUSEDIAG: {
		struct x25_causediag causediag;
		rc = -EFAULT;
		if (copy_from_user(&causediag, argp, sizeof(causediag)))
			break;
		lock_sock(sk);
		x25->causediag = causediag;
		release_sock(sk);
		rc = 0;
		break;

	}

	case SIOCX25SCUDMATCHLEN: {
		struct x25_subaddr sub_addr;
		rc = -EINVAL;
		lock_sock(sk);
		if(sk->sk_state != TCP_CLOSE)
			goto out_cud_release;
		rc = -EFAULT;
		if (copy_from_user(&sub_addr, argp,
				   sizeof(sub_addr)))
			goto out_cud_release;
		rc = -EINVAL;
		if (sub_addr.cudmatchlength > X25_MAX_CUD_LEN)
			goto out_cud_release;
		x25->cudmatchlength = sub_addr.cudmatchlength;
		rc = 0;
out_cud_release:
		release_sock(sk);
		break;
	}

	case SIOCX25CALLACCPTAPPRV: {
		rc = -EINVAL;
		lock_sock(sk);
		if (sk->sk_state == TCP_CLOSE) {
			clear_bit(X25_ACCPT_APPRV_FLAG, &x25->flags);
			rc = 0;
		}
		release_sock(sk);
		break;
	}

	case SIOCX25SENDCALLACCPT:  {
		rc = -EINVAL;
		lock_sock(sk);
		if (sk->sk_state != TCP_ESTABLISHED)
			goto out_sendcallaccpt_release;
		/* must call accptapprv above */
		if (test_bit(X25_ACCPT_APPRV_FLAG, &x25->flags))
			goto out_sendcallaccpt_release;
		x25_write_internal(sk, X25_CALL_ACCEPTED);
		x25->state = X25_STATE_3;
		rc = 0;
out_sendcallaccpt_release:
		release_sock(sk);
		break;
	}

	default:
		rc = -ENOIOCTLCMD;
		break;
	}

	return rc;
}