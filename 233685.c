static int irda_accept(struct socket *sock, struct socket *newsock, int flags)
{
	struct sock *sk = sock->sk;
	struct irda_sock *new, *self = irda_sk(sk);
	struct sock *newsk;
	struct sk_buff *skb;
	int err;

	IRDA_DEBUG(2, "%s()\n", __func__);

	err = irda_create(sock_net(sk), newsock, sk->sk_protocol);
	if (err)
		return err;

	if (sock->state != SS_UNCONNECTED)
		return -EINVAL;

	if ((sk = sock->sk) == NULL)
		return -EINVAL;

	if ((sk->sk_type != SOCK_STREAM) && (sk->sk_type != SOCK_SEQPACKET) &&
	    (sk->sk_type != SOCK_DGRAM))
		return -EOPNOTSUPP;

	if (sk->sk_state != TCP_LISTEN)
		return -EINVAL;

	/*
	 *	The read queue this time is holding sockets ready to use
	 *	hooked into the SABM we saved
	 */

	/*
	 * We can perform the accept only if there is incoming data
	 * on the listening socket.
	 * So, we will block the caller until we receive any data.
	 * If the caller was waiting on select() or poll() before
	 * calling us, the data is waiting for us ;-)
	 * Jean II
	 */
	while (1) {
		skb = skb_dequeue(&sk->sk_receive_queue);
		if (skb)
			break;

		/* Non blocking operation */
		if (flags & O_NONBLOCK)
			return -EWOULDBLOCK;

		err = wait_event_interruptible(*(sk->sk_sleep),
					skb_peek(&sk->sk_receive_queue));
		if (err)
			return err;
	}

	newsk = newsock->sk;
	if (newsk == NULL)
		return -EIO;

	newsk->sk_state = TCP_ESTABLISHED;

	new = irda_sk(newsk);

	/* Now attach up the new socket */
	new->tsap = irttp_dup(self->tsap, new);
	if (!new->tsap) {
		IRDA_DEBUG(0, "%s(), dup failed!\n", __func__);
		kfree_skb(skb);
		return -1;
	}

	new->stsap_sel = new->tsap->stsap_sel;
	new->dtsap_sel = new->tsap->dtsap_sel;
	new->saddr = irttp_get_saddr(new->tsap);
	new->daddr = irttp_get_daddr(new->tsap);

	new->max_sdu_size_tx = self->max_sdu_size_tx;
	new->max_sdu_size_rx = self->max_sdu_size_rx;
	new->max_data_size   = self->max_data_size;
	new->max_header_size = self->max_header_size;

	memcpy(&new->qos_tx, &self->qos_tx, sizeof(struct qos_info));

	/* Clean up the original one to keep it in listen state */
	irttp_listen(self->tsap);

	kfree_skb(skb);
	sk->sk_ack_backlog--;

	newsock->state = SS_CONNECTED;

	irda_connect_response(new);

	return 0;
}