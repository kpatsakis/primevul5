int svc_change_qos(struct atm_vcc *vcc,struct atm_qos *qos)
{
	struct sock *sk = sk_atm(vcc);
	DEFINE_WAIT(wait);

	set_bit(ATM_VF_WAITING, &vcc->flags);
	prepare_to_wait(sk->sk_sleep, &wait, TASK_UNINTERRUPTIBLE);
	sigd_enq2(vcc,as_modify,NULL,NULL,&vcc->local,qos,0);
	while (test_bit(ATM_VF_WAITING, &vcc->flags) &&
	       !test_bit(ATM_VF_RELEASED, &vcc->flags) && sigd) {
		schedule();
		prepare_to_wait(sk->sk_sleep, &wait, TASK_UNINTERRUPTIBLE);
	}
	finish_wait(sk->sk_sleep, &wait);
	if (!sigd) return -EUNATCH;
	return -sk->sk_err;
}