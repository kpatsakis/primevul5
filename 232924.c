struct sk_buff *__skb_recv_udp(struct sock *sk, unsigned int flags,
			       int noblock, int *peeked, int *off, int *err)
{
	struct sk_buff_head *sk_queue = &sk->sk_receive_queue;
	struct sk_buff_head *queue;
	struct sk_buff *last;
	long timeo;
	int error;

	queue = &udp_sk(sk)->reader_queue;
	flags |= noblock ? MSG_DONTWAIT : 0;
	timeo = sock_rcvtimeo(sk, flags & MSG_DONTWAIT);
	do {
		struct sk_buff *skb;

		error = sock_error(sk);
		if (error)
			break;

		error = -EAGAIN;
		*peeked = 0;
		do {
			spin_lock_bh(&queue->lock);
			skb = __skb_try_recv_from_queue(sk, queue, flags,
							udp_skb_destructor,
							peeked, off, err,
							&last);
			if (skb) {
				spin_unlock_bh(&queue->lock);
				return skb;
			}

			if (skb_queue_empty(sk_queue)) {
				spin_unlock_bh(&queue->lock);
				goto busy_check;
			}

			/* refill the reader queue and walk it again
			 * keep both queues locked to avoid re-acquiring
			 * the sk_receive_queue lock if fwd memory scheduling
			 * is needed.
			 */
			spin_lock(&sk_queue->lock);
			skb_queue_splice_tail_init(sk_queue, queue);

			skb = __skb_try_recv_from_queue(sk, queue, flags,
							udp_skb_dtor_locked,
							peeked, off, err,
							&last);
			spin_unlock(&sk_queue->lock);
			spin_unlock_bh(&queue->lock);
			if (skb)
				return skb;

busy_check:
			if (!sk_can_busy_loop(sk))
				break;

			sk_busy_loop(sk, flags & MSG_DONTWAIT);
		} while (!skb_queue_empty(sk_queue));

		/* sk_queue is empty, reader_queue may contain peeked packets */
	} while (timeo &&
		 !__skb_wait_for_more_packets(sk, &error, &timeo,
					      (struct sk_buff *)sk_queue));

	*err = error;
	return NULL;
}