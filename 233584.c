static void svc_disconnect(struct atm_vcc *vcc)
{
	DEFINE_WAIT(wait);
	struct sk_buff *skb;
	struct sock *sk = sk_atm(vcc);

	pr_debug("svc_disconnect %p\n",vcc);
	if (test_bit(ATM_VF_REGIS,&vcc->flags)) {
		prepare_to_wait(sk->sk_sleep, &wait, TASK_UNINTERRUPTIBLE);
		sigd_enq(vcc,as_close,NULL,NULL,NULL);
		while (!test_bit(ATM_VF_RELEASED,&vcc->flags) && sigd) {
			schedule();
			prepare_to_wait(sk->sk_sleep, &wait, TASK_UNINTERRUPTIBLE);
		}
		finish_wait(sk->sk_sleep, &wait);
	}
	/* beware - socket is still in use by atmsigd until the last
	   as_indicate has been answered */
	while ((skb = skb_dequeue(&sk->sk_receive_queue)) != NULL) {
		atm_return(vcc, skb->truesize);
		pr_debug("LISTEN REL\n");
		sigd_enq2(NULL,as_reject,vcc,NULL,NULL,&vcc->qos,0);
		dev_kfree_skb(skb);
	}
	clear_bit(ATM_VF_REGIS, &vcc->flags);
	/* ... may retry later */
}