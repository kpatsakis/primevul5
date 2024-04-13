static int svc_addparty(struct socket *sock, struct sockaddr *sockaddr,
			int sockaddr_len, int flags)
{
	DEFINE_WAIT(wait);
	struct sock *sk = sock->sk;
	struct atm_vcc *vcc = ATM_SD(sock);
	int error;

	lock_sock(sk);
	set_bit(ATM_VF_WAITING, &vcc->flags);
	prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
	sigd_enq(vcc, as_addparty, NULL, NULL,
		 (struct sockaddr_atmsvc *) sockaddr);
	if (flags & O_NONBLOCK) {
		finish_wait(sk->sk_sleep, &wait);
		error = -EINPROGRESS;
		goto out;
	}
	pr_debug("svc_addparty added wait queue\n");
	while (test_bit(ATM_VF_WAITING, &vcc->flags) && sigd) {
		schedule();
		prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
	}
	finish_wait(sk->sk_sleep, &wait);
	error = xchg(&sk->sk_err_soft, 0);
out:
	release_sock(sk);
	return error;
}