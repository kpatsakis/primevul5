static int x25_accept(struct socket *sock, struct socket *newsock, int flags,
		      bool kern)
{
	struct sock *sk = sock->sk;
	struct sock *newsk;
	struct sk_buff *skb;
	int rc = -EINVAL;

	if (!sk)
		goto out;

	rc = -EOPNOTSUPP;
	if (sk->sk_type != SOCK_SEQPACKET)
		goto out;

	lock_sock(sk);
	rc = -EINVAL;
	if (sk->sk_state != TCP_LISTEN)
		goto out2;

	rc = x25_wait_for_data(sk, sk->sk_rcvtimeo);
	if (rc)
		goto out2;
	skb = skb_dequeue(&sk->sk_receive_queue);
	rc = -EINVAL;
	if (!skb->sk)
		goto out2;
	newsk		 = skb->sk;
	sock_graft(newsk, newsock);

	/* Now attach up the new socket */
	skb->sk = NULL;
	kfree_skb(skb);
	sk_acceptq_removed(sk);
	newsock->state = SS_CONNECTED;
	rc = 0;
out2:
	release_sock(sk);
out:
	return rc;
}