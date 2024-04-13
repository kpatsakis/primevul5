static int svc_accept(struct socket *sock,struct socket *newsock,int flags)
{
	struct sock *sk = sock->sk;
	struct sk_buff *skb;
	struct atmsvc_msg *msg;
	struct atm_vcc *old_vcc = ATM_SD(sock);
	struct atm_vcc *new_vcc;
	int error;

	lock_sock(sk);

	error = svc_create(sock_net(sk), newsock,0);
	if (error)
		goto out;

	new_vcc = ATM_SD(newsock);

	pr_debug("svc_accept %p -> %p\n",old_vcc,new_vcc);
	while (1) {
		DEFINE_WAIT(wait);

		prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
		while (!(skb = skb_dequeue(&sk->sk_receive_queue)) &&
		       sigd) {
			if (test_bit(ATM_VF_RELEASED,&old_vcc->flags)) break;
			if (test_bit(ATM_VF_CLOSE,&old_vcc->flags)) {
				error = -sk->sk_err;
				break;
			}
			if (flags & O_NONBLOCK) {
				error = -EAGAIN;
				break;
			}
			release_sock(sk);
			schedule();
			lock_sock(sk);
			if (signal_pending(current)) {
				error = -ERESTARTSYS;
				break;
			}
			prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
		}
		finish_wait(sk->sk_sleep, &wait);
		if (error)
			goto out;
		if (!skb) {
			error = -EUNATCH;
			goto out;
		}
		msg = (struct atmsvc_msg *) skb->data;
		new_vcc->qos = msg->qos;
		set_bit(ATM_VF_HASQOS,&new_vcc->flags);
		new_vcc->remote = msg->svc;
		new_vcc->local = msg->local;
		new_vcc->sap = msg->sap;
		error = vcc_connect(newsock, msg->pvc.sap_addr.itf,
				    msg->pvc.sap_addr.vpi, msg->pvc.sap_addr.vci);
		dev_kfree_skb(skb);
		sk->sk_ack_backlog--;
		if (error) {
			sigd_enq2(NULL,as_reject,old_vcc,NULL,NULL,
			    &old_vcc->qos,error);
			error = error == -EAGAIN ? -EBUSY : error;
			goto out;
		}
		/* wait should be short, so we ignore the non-blocking flag */
		set_bit(ATM_VF_WAITING, &new_vcc->flags);
		prepare_to_wait(sk_atm(new_vcc)->sk_sleep, &wait, TASK_UNINTERRUPTIBLE);
		sigd_enq(new_vcc,as_accept,old_vcc,NULL,NULL);
		while (test_bit(ATM_VF_WAITING, &new_vcc->flags) && sigd) {
			release_sock(sk);
			schedule();
			lock_sock(sk);
			prepare_to_wait(sk_atm(new_vcc)->sk_sleep, &wait, TASK_UNINTERRUPTIBLE);
		}
		finish_wait(sk_atm(new_vcc)->sk_sleep, &wait);
		if (!sigd) {
			error = -EUNATCH;
			goto out;
		}
		if (!sk_atm(new_vcc)->sk_err)
			break;
		if (sk_atm(new_vcc)->sk_err != ERESTARTSYS) {
			error = -sk_atm(new_vcc)->sk_err;
			goto out;
		}
	}
	newsock->state = SS_CONNECTED;
out:
	release_sock(sk);
	return error;
}