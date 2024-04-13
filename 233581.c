static int svc_connect(struct socket *sock,struct sockaddr *sockaddr,
    int sockaddr_len,int flags)
{
	DEFINE_WAIT(wait);
	struct sock *sk = sock->sk;
	struct sockaddr_atmsvc *addr;
	struct atm_vcc *vcc = ATM_SD(sock);
	int error;

	pr_debug("svc_connect %p\n",vcc);
	lock_sock(sk);
	if (sockaddr_len != sizeof(struct sockaddr_atmsvc)) {
		error = -EINVAL;
		goto out;
	}

	switch (sock->state) {
	default:
		error = -EINVAL;
		goto out;
	case SS_CONNECTED:
		error = -EISCONN;
		goto out;
	case SS_CONNECTING:
		if (test_bit(ATM_VF_WAITING, &vcc->flags)) {
			error = -EALREADY;
			goto out;
		}
		sock->state = SS_UNCONNECTED;
		if (sk->sk_err) {
			error = -sk->sk_err;
			goto out;
		}
		break;
	case SS_UNCONNECTED:
		addr = (struct sockaddr_atmsvc *) sockaddr;
		if (addr->sas_family != AF_ATMSVC) {
			error = -EAFNOSUPPORT;
			goto out;
		}
		if (!test_bit(ATM_VF_HASQOS, &vcc->flags)) {
			error = -EBADFD;
			goto out;
		}
		if (vcc->qos.txtp.traffic_class == ATM_ANYCLASS ||
		    vcc->qos.rxtp.traffic_class == ATM_ANYCLASS) {
			error = -EINVAL;
			goto out;
		}
		if (!vcc->qos.txtp.traffic_class &&
		    !vcc->qos.rxtp.traffic_class) {
			error = -EINVAL;
			goto out;
		}
		vcc->remote = *addr;
		set_bit(ATM_VF_WAITING, &vcc->flags);
		prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
		sigd_enq(vcc,as_connect,NULL,NULL,&vcc->remote);
		if (flags & O_NONBLOCK) {
			finish_wait(sk->sk_sleep, &wait);
			sock->state = SS_CONNECTING;
			error = -EINPROGRESS;
			goto out;
		}
		error = 0;
		while (test_bit(ATM_VF_WAITING, &vcc->flags) && sigd) {
			schedule();
			if (!signal_pending(current)) {
				prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
				continue;
			}
			pr_debug("*ABORT*\n");
			/*
			 * This is tricky:
			 *   Kernel ---close--> Demon
			 *   Kernel <--close--- Demon
			 * or
			 *   Kernel ---close--> Demon
			 *   Kernel <--error--- Demon
			 * or
			 *   Kernel ---close--> Demon
			 *   Kernel <--okay---- Demon
			 *   Kernel <--close--- Demon
			 */
			sigd_enq(vcc,as_close,NULL,NULL,NULL);
			while (test_bit(ATM_VF_WAITING, &vcc->flags) && sigd) {
				prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
				schedule();
			}
			if (!sk->sk_err)
				while (!test_bit(ATM_VF_RELEASED,&vcc->flags)
				    && sigd) {
					prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
					schedule();
				}
			clear_bit(ATM_VF_REGIS,&vcc->flags);
			clear_bit(ATM_VF_RELEASED,&vcc->flags);
			clear_bit(ATM_VF_CLOSE,&vcc->flags);
			    /* we're gone now but may connect later */
			error = -EINTR;
			break;
		}
		finish_wait(sk->sk_sleep, &wait);
		if (error)
			goto out;
		if (!sigd) {
			error = -EUNATCH;
			goto out;
		}
		if (sk->sk_err) {
			error = -sk->sk_err;
			goto out;
		}
	}
/*
 * Not supported yet
 *
 * #ifndef CONFIG_SINGLE_SIGITF
 */
	vcc->qos.txtp.max_pcr = SELECT_TOP_PCR(vcc->qos.txtp);
	vcc->qos.txtp.pcr = 0;
	vcc->qos.txtp.min_pcr = 0;
/*
 * #endif
 */
	if (!(error = vcc_connect(sock, vcc->itf, vcc->vpi, vcc->vci)))
		sock->state = SS_CONNECTED;
	else
		(void) svc_disconnect(vcc);
out:
	release_sock(sk);
	return error;
}