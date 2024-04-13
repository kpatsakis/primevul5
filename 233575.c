static int svc_bind(struct socket *sock,struct sockaddr *sockaddr,
    int sockaddr_len)
{
	DEFINE_WAIT(wait);
	struct sock *sk = sock->sk;
	struct sockaddr_atmsvc *addr;
	struct atm_vcc *vcc;
	int error;

	if (sockaddr_len != sizeof(struct sockaddr_atmsvc))
		return -EINVAL;
	lock_sock(sk);
	if (sock->state == SS_CONNECTED) {
		error = -EISCONN;
		goto out;
	}
	if (sock->state != SS_UNCONNECTED) {
		error = -EINVAL;
		goto out;
	}
	vcc = ATM_SD(sock);
	addr = (struct sockaddr_atmsvc *) sockaddr;
	if (addr->sas_family != AF_ATMSVC) {
		error = -EAFNOSUPPORT;
		goto out;
	}
	clear_bit(ATM_VF_BOUND,&vcc->flags);
	    /* failing rebind will kill old binding */
	/* @@@ check memory (de)allocation on rebind */
	if (!test_bit(ATM_VF_HASQOS,&vcc->flags)) {
		error = -EBADFD;
		goto out;
	}
	vcc->local = *addr;
	set_bit(ATM_VF_WAITING, &vcc->flags);
	prepare_to_wait(sk->sk_sleep, &wait, TASK_UNINTERRUPTIBLE);
	sigd_enq(vcc,as_bind,NULL,NULL,&vcc->local);
	while (test_bit(ATM_VF_WAITING, &vcc->flags) && sigd) {
		schedule();
		prepare_to_wait(sk->sk_sleep, &wait, TASK_UNINTERRUPTIBLE);
	}
	finish_wait(sk->sk_sleep, &wait);
	clear_bit(ATM_VF_REGIS,&vcc->flags); /* doesn't count */
	if (!sigd) {
		error = -EUNATCH;
		goto out;
	}
	if (!sk->sk_err)
		set_bit(ATM_VF_BOUND,&vcc->flags);
	error = -sk->sk_err;
out:
	release_sock(sk);
	return error;
}