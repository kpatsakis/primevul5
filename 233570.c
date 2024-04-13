static int svc_dropparty(struct socket *sock, int ep_ref)
{
	DEFINE_WAIT(wait);
	struct sock *sk = sock->sk;
	struct atm_vcc *vcc = ATM_SD(sock);
	int error;

	lock_sock(sk);
	set_bit(ATM_VF_WAITING, &vcc->flags);
	prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
	sigd_enq2(vcc, as_dropparty, NULL, NULL, NULL, NULL, ep_ref);
	while (test_bit(ATM_VF_WAITING, &vcc->flags) && sigd) {
		schedule();
		prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
	}
	finish_wait(sk->sk_sleep, &wait);
	if (!sigd) {
		error = -EUNATCH;
		goto out;
	}
	error = xchg(&sk->sk_err_soft, 0);
out:
	release_sock(sk);
	return error;
}